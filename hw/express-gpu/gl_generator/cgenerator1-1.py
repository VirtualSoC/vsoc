#!/bin/python3
# -*- coding: UTF-8 -*-

# Input file name
filename = "2.4.txt"

# 64-bit target?
target64bit = True

ptrbits = 64 if target64bit else 32
sizeof_dic = {
    'GLboolean':32,     'GLbyte':8,           'GLubyte':8,
    'GLshort':16,       'GLushort':16,        'GLint':32,
    'GLuint':32,        'GLfixed':32,         'GLint64':64,
    'GLuint64':64,      'GLsizei':32,         'GLenum':32,
    'GLintptr':ptrbits, 'GLsizeiptr':ptrbits, 'GLsync':ptrbits,
    'GLbitfield':32,    'GLhalf':16,          'GLfloat':32,
    'GLclampf':32,      'GLdouble':64,        'GLclampd':64,
    'GLchar':8,
    # GLchar is not defined in documents, but is defined as 'char' in header
    # files
}
def sizeof(arg_type):
    return sizeof_dic[arg_type]

f = open(filename, "r")
print(f"\n\n/******* file '{filename}' *******/\n\n")

for line in f:

    temp = line.split()

    # Empty line means end of file
    if len(temp) == 0:
        break

    offset = 0
    asynchronous = True

    # Check if there is a return value
    ret_val = ""
    if temp[0][:2] != 'gl':
        if temp[0] == 'const':
            ret_val = ' '.join(temp[:2])
            offset = 2
        else:
            ret_val = temp[0]
            offset = 1

    # Get function name
    func_name = temp[offset]
    offset += 1

    non_ptr_args = []
    in_ptr_args = []
    out_ptr_args = []
    arg_list = []

    # Read arguments
    if temp[-1] != 'void':
        while offset < len(temp):
            ptr = 'NA'

            # Const arguments are inputs
            if temp[offset] == 'const':
                typ = ' '.join(temp[offset:offset+2])
                name = temp[offset+2]
                ptr = 'in'
                offset += 3

            else:
                typ = temp[offset]
                name = temp[offset+1]
                offset += 2
                if name[0] == '*':
                    ptr = 'out'

            if name[-1] == ',':
                name = name[:-1]

            arg_list += [{'type': typ, 'name': name, 'ptr':ptr}]

        non_ptr_args = [arg for arg in arg_list if arg['ptr'] == 'NA']
        in_ptr_args = [arg for arg in arg_list if arg['ptr'] == 'in']
        out_ptr_args = [arg for arg in arg_list if arg['ptr'] == 'out']

    # Debug information
    print()
    print(f"/** ----- readline: {line[:-1]}**/")
    print(f"/** ----- func name: {func_name}**/")
    print(f"/** ----- arg_list: {arg_list}**/")
    print(f"/** ----- ret_val: {ret_val} **/")
    print()

    # Case
    print(f"case FUNID_{func_name}:\n")

    # If there are more than one ptr, require check
    if len(in_ptr_args) + len(out_ptr_args) > 1:
        print(f"/* TODO: More than one ptr, should check mannually */")

    # Define non-pointer variables
    print("/* Define variables */")
    for arg in non_ptr_args:
        print(f"{arg['type']} {arg['name']};")
    header_str = \
"""
MYGPU_Queue_Elem *header = call->elem_header;
MYGPU_Queue_Elem *now_elem = header->next;
if(now_elem == NULL){
    //call->callback(call,0);
    break;
}
GLbyte *temp = now_elem->para;
if(temp == NULL){
    //call->callback(call,0);
    break;
}
"""
    print(header_str)

    # Is there an array to copy?
    in_ptr_from_stream = False
    for arg in in_ptr_args:
        if arg['type'] != 'const void' and arg['type'] != 'const GLchar':
            if in_ptr_from_stream:
                print("/* TODO: More than one input array! Should check */")
                break
            in_ptr_from_stream = True

    # Read non-ptr arguments and input arries from stream
    # There should be no more than one input array (except char* or void*)
    if len(non_ptr_args) > 0 or in_ptr_from_stream:

        # Get size of non ptr arguments
        non_ptr_args_length = 0
        for arg in non_ptr_args:
            non_ptr_args_length += sizeof(arg['type']) / sizeof('GLbyte')

        # If there is an input array
        if in_ptr_from_stream:

            # Get size of ptr arguments
            ptr_length = ''
            for arg in non_ptr_args:
                if arg['type'] == 'GLsizei':
                    if ptr_length != '':
                        print("/* TODO: More than one GLsizei type argument! "\
                                "Should check */")
                        break
                    ptr_length = arg['name']

            # For glClearBuffer, the length of value depends on buffer
            if ptr_length == '':
                print("/* TODO: no length found, please check */")
                ptr_length = 'NA'

            args_length_str = f"({int(non_ptr_args_length)}+{ptr_length})"
        
        else:
            args_length_str = f"{int(non_ptr_args_length)}"

        # Check length
        check_len_str = \
f"""/* Check length */
size_t temp_len=now_elem->len;
if(temp_len != {args_length_str} * sizeof(GLbyte)){{
    //call->callback(call,0);
    break;
}}
"""
        print(check_len_str)

        # Read variables
        print("/* Read variables */")
        i = 0
        for arg in arg_list:
            if arg['ptr'] == 'NA':
                print(f"{arg['name']} = ({arg['type']})temp[{int(i)}];")
                i += sizeof(arg['type']) / sizeof('GLbyte')
            elif arg['ptr'] == 'in' and \
                    arg['type'] != 'const void' and \
                    arg['type'] != 'const GLchar':
                read_array_str = \
f"""/* Copy input array */
{arg['type']} {arg['name']};
if({ptr_length} < 1024){{
    {arg['name']} = buf;
    memcpy(buf, temp+{i}, {ptr_length}*sizeof({arg['type']}));
}}else{{
    {arg['name']} = g_malloc(n*sizeof(GLint));
    memcpy({arg['name']}, temp+{i}, {ptr_length}*sizeof({arg['type']}));
}}"""
                print(read_array_str)
        print()

    # Must do synchronous to wait for output or return value.
    if len(out_ptr_args) > 0 or ret_val != '':
        asynchronous = False

    # If there is an input argument that cannot be copied.
    for arg in in_ptr_args:
        # Can't get the length of c-style string immediately.
        # Void* data can be long binaries or arries with unknown length (may
        # even be a number as indices in glDrawElements).
        if arg['type'] == 'const GLchar' or arg['type'] == 'const void':
            asynchronous = False
            break

    # Get call string
    call_str = f"{func_name}("
    for arg in arg_list:
        call_str += f"{arg['name']}, "
    call_str = call_str[:-2].replace('*', '') + ")"

    # Do asynchronous call
    if asynchronous:

        # Callback
        print("call->callback(call);")

        # Run function
        run_func_str = call_str + ";"
        print(run_func_str)

    # Do synchronous call
    else:

        # Pointer arguments
        for arg in arg_list:

            # Deal with output ptr arguments
            if arg['ptr'] == 'out':
                out_ptr_str = \
f"""/* Output ptr */
now_elem = now_elem->next;
if(now_elem == NULL){{
    call->callback(call);
    break;
}}
now_elem->type |= RET_PARA;
{arg['type']} {arg['name']} = ({arg['type']}*) now_elem->para;
if({arg['name'][1:]} == NULL){{
    call->callback(call);
    break;
}}
"""
                print(out_ptr_str)

            # Deal with input ptr arguments not copied
            if arg['ptr'] == 'in':
                in_ptr_str = \
f"""/* Input array that shouldn't be copied */
now_elem = now_elem->next;
if(now_elem == NULL){{
    call->callback(call);
    break;
}}
{arg['type']} {arg['name']}=({arg['type']} *)now_elem->para;
"""
                print(in_ptr_str)

        # Deal with return value
        # TODO: if return value is a string...
        if ret_val != '':
            if ret_val[-1] == '*':
                run_func_str = \
f"""/* This function has a return value */
{ret_val} ret_buf = ({ret_val})now_elem->para;
if(ret_buf == NULL){{
    call->callback(call);
    break;
}}
{ret_val} recv_buf = glGetString(name);
strcpy(ret_buf, recv_buf);
"""
            else:
                run_func_str = \
f"""/* This function has a return value */
{ret_val} ret = {call_str};
MYGPU_Flag_Buf *ret_buf = (MYGPU_Flag_Buf *)header->para;
*(ret_val *)(&(ret_buf->ret)) = ret;
"""

        else:
            run_func_str = call_str + ";"

        # Run function
        print(run_func_str)

        # Callback
        print("call->callback(call);")

    print("break;")

# End for

print(f"\n\n/******* end of file '{filename}' *******/\n\n")
