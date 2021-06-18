#!/bin/python3
# -*- coding: UTF-8 -*-

import sys

# gl_pixel_data_size(width,height,format,type)
filenames = [
    "1-1",
    "1-3",
    "2-1",
    "2-2",
    '2-3',
    "2-4",
    "2-5",
    "2-6",
]

# 64-bit target?
target64bit = True

ptrbits = 64 if target64bit else 32
sizeof_dic = {
    'GLboolean': 32,     'GLbyte': 8,           'GLubyte': 8,
    'GLshort': 16,       'GLushort': 16,        'GLint': 32,
    'GLuint': 32,        'GLfixed': 32,         'GLint64': 64,
    'GLuint64': 64,      'GLsizei': 32,         'GLenum': 32,
    'GLintptr': ptrbits, 'GLsizeiptr': ptrbits, 'GLsync': ptrbits,
    'GLeglImageOES': ptrbits,
    'GLbitfield': 32,    'GLhalf': 16,          'GLfloat': 32,
    'GLclampf': 32,      'GLdouble': 64,        'GLclampd': 64,
    'GLchar': 8,         'GLclampx': 32,
    # GLchar is not defined in documents, but is defined as 'char' in header
    # files
}

all_include=open(f"all_gl.h","w+")
all_include.write('#include "glad.h"\n')

def sizeof(arg_type):
    return int(sizeof_dic[arg_type] / sizeof_dic['GLbyte'])


class Func:
    def __init__(self, line, file_type):

        self.line = line.strip()

        self.name = ""
        self.ret = ""
        self.args = []
        self.non_ptr_args = []
        self.in_ptr_args = []
        self.out_ptr_args = []  # ptr used as a return value

        self.PtrOfPtr = False
        # self.stream_array_name = "" # Array wrote into non ptr stream
        # self.stream_array_len = ""
        if file_type == 1:
            self.asyn = True
        else:
            self.asyn = False


        self.args_num = 0
        self.send_items_num = 0


        self.read_def()

    def read_def(self):

        # print(self.line)
        temp = self.line.split()

        # Check if there is a return value
        offset = 0
        if temp[0][:2] != 'gl':
            if temp[0] == 'const':
                self.ret = ' '.join(temp[:2])
                offset = 2
            else:
                self.ret = temp[0]
                offset = 1

        # Get function name
        self.name = temp[offset]
        offset += 1

        # Read arguments
        if temp[-1] != 'void':
            flag_out = 0
            argu_cnt = 0
            while offset < len(temp):
                ptr = 'NA'
                ptr_len = 'NA'

                # Const arguments are inputs
                if temp[offset] == 'const':
                    if flag_out == 1:
                        print("error!!!! out before in!!!")
                    typ = ' '.join(temp[offset:offset+2]) + "*"
                    name = temp[offset+2][1:]
                    if name[-1] == ',':
                        name = name[:-1]
                    ptr = 'in'
                    offset += 3
                    if name.find("#") != -1:
                        name_len = name.split("#")
                        name = name_len[0]
                        ptr_len = name_len[1]
                    # if name[:6] == 'const*':
                    #     self.errorParsing.StarConstStar = True

                else:
                    typ = temp[offset]
                    # print(temp)
                    name = temp[offset+1]
                    if name[-1] == ',':
                        name = name[:-1]
                    offset += 2
                    if name[0] == '*':
                        flag_out = 1
                        ptr = 'out'
                        typ += "*"
                        name = name[1:]
                        if name.find("#") != -1:
                            name_len = name.split("#")
                            name = name_len[0]
                            ptr_len = name_len[1]

                # ptr of ptr

                PtrOfPtr = False
                if name[:6] == 'const*':
                    self.PtrOfPtr = True
                    PtrOfPtr = True
                    name = name[6:]

                # while name[0] == '*':
                #     self.errorParsing.PtrOfPtr = True
                #     typ += "*"
                #     name = name[1:]

                if name[-1] == ',':
                    name = name[:-1]

                self.args += [{'type': typ, 'name': name, 'ptr': ptr,
                               'ptr_len': ptr_len, "loc": argu_cnt, 'ptr_ptr': PtrOfPtr}]
                argu_cnt += 1

            self.non_ptr_args = [
                arg for arg in self.args if arg['ptr'] == 'NA']
            self.in_ptr_args = [arg for arg in self.args if arg['ptr'] == 'in']
            self.out_ptr_args = [
                arg for arg in self.args if arg['ptr'] == 'out']
            self.args_num = len(self.args)
            self.send_items_num = len(self.in_ptr_args)
            self.send_items_num += len(self.out_ptr_args)
            if len(self.non_ptr_args) != 0:
                self.send_items_num += 1
            if self.asyn and self.send_items_num > 1:
                self.send_items_num = 1
        if self.ret != "":
            self.send_items_num += 1

        # Must do synchronous to wait for output or return value.
        if len(self.out_ptr_args) > 0 or self.ret != '':
            self.asyn = False

        #  # If there is an input argument that cannot be copied.
        #  for arg in self.in_ptr_args:
            #  # Can't get the length of c-style string immediately.
            #  # Void* data can be long binaries or arries with unknown length (may
            #  # even be a number as indices in glDrawElements).
            #  if arg['type'] == 'const GLchar*' or arg['type'] == 'const void*':
            #  self.asyn = False
            #  break

        # Is there an array written into non ptr stream?
        # stream_array = ''
        # for arg in self.in_ptr_args:
        #     if arg['type'] != 'const void*' and arg['type'] != 'const GLchar*':
        #         stream_array = arg['type'].split()[-1] # without 'const'
        #         self.stream_array_name = arg['name']

        #         # Get size of ptr arguments
        #         self.errorParsing.GuessUsingGLsizei = True

        #         for size_arg in self.non_ptr_args:
        #             # Assuming GLsizei is array length
        #             if size_arg['type'] == 'GLsizei':
        #                 self.stream_array_len = f"{size_arg['name']}*" + \
        #                         f"{sizeof(stream_array[:-1])}"
        #                 break
        #         break

    def get_non_ptr_arg_length(self):
        # Get size of non ptr arguments
        non_ptr_arg_length = 0
        for arg in self.non_ptr_args:
            non_ptr_arg_length += sizeof(arg['type'])
        return non_ptr_arg_length

    def generate_qemu_case(self, out_file, define_id):
        global define_line
        global para_num_line
        # qemu_define.write(f"#define FUNID_{self.name} ((EXPRESS_GPU_FUN_ID<<16)+{define_id})\n")
        define_line += f"#define FUNID_{self.name} ((EXPRESS_GPU_FUN_ID<<16)+{define_id})\n"

        para_num = len(self.in_ptr_args)+len(self.out_ptr_args)
        if len(self.non_ptr_args) != 0:
            para_num += 1

        # qemu_para_num.write(f"#define PARA_NUM_MIN_{self.name} ({para_num})\n")
        para_num_line += f"#define PARA_NUM_MIN_{self.name} ({self.send_items_num})\n"

        # Case
        out_file.write(f"case FUNID_{self.name}:\n\n{{\n")
        # Debug information
        out_file.write("\n")
        out_file.write(f"/* readline: \"{self.line}\" */\n")
        out_file.write(f"/* func name: \"{self.name}\" */\n")
        out_file.write(f"/* args: {self.args} */\n")
        out_file.write(f"/* ret: \"{self.ret}\" */\n")
        out_file.write(f"/* asyn: \"{self.asyn}\" */\n")
        out_file.write("\n")

        # If there are more than one ptr, require checking
        if len(self.in_ptr_args) + len(self.out_ptr_args) > 1:
            out_file.write(
                f"/* TODO: More than one ptr, should check mannually */\n")

        # Define non-pointer variables
        out_file.write("/* Define variables */\n")
        for arg in self.non_ptr_args:
            out_file.write(f"{arg['type']} {arg['name']};\n")
        header_str = \
            f"""
int para_num=get_para_from_call(call,all_para,MAX_PARA_NUM);
if(para_num<PARA_NUM_MIN_{self.name}){{
    break;
}}
int para_loc=0;
size_t temp_len;
char *temp;
"""
        out_file.write(header_str)

        # Read non-ptr arguments and input arrays from stream
        if self.send_items_num == 1:
            if len(self.non_ptr_args) > 0 or self.asyn:
                args_length_str = f"{int(self.get_non_ptr_arg_length())}"

                # Check length
                check_len_str = \
                    f"""/* Check length */
temp_len=all_para[para_loc].data_len;
guest_write(all_para[para_loc].data,num_buf,0,temp_len);

if(temp_len < {args_length_str} * {sizeof('GLbyte')}){{
    break;
}}\n
"""
                out_file.write(check_len_str)

                # Read variables
                out_file.write("/* Read variables */\n")
                i = "0"
                for arg in self.args:
                    # Read non-ptr variable
                    if arg['ptr'] == 'NA':
                        out_file.write(f"{arg['name']} ="
                                       f" *({arg['type']} *)(num_buf+{i});\n")
                        i += "+"+str(sizeof(arg['type']))
                #异步函数所有的数据都要存储到一个缓冲区中，所以都要从一个缓冲区中取数据
                for arg in self.args:
                    # Read ptr variable
                    if arg['ptr'] == 'in' and self.asyn:
                        if not arg['ptr_ptr']:
                                            # Check length
                            
                            out_file.write(
                                f"""{arg['type']} {arg['name']}=({arg['type']})(num_buf+{i});\n""")
                            i += "+"+arg['ptr_len']
                            check_len_str = \
f"""/* Check length */
if(temp_len < {args_length_str} * {sizeof('GLbyte')}+{arg['ptr_len']}){{
    break;
}}\n
"""
                            out_file.write(check_len_str)
                        else:
                            print("error need handle")

        # Deal with input ptr arguments not copied
        free_thing = []
        for arg in self.args:
            # if arg['type'] == 'const void*' or arg['type'] == 'const GLchar*':
            if arg['ptr'] == "in":

                if arg['ptr_ptr']:
                    if self.asyn:
                        #异步函数不能有指针的指针，它的所有数据都存储到一个缓冲区里，并且只有输入
                        print("error ptr of ptr when asyn qemu")
                    free_flag = True
                    in_ptr_str = \
    f"""
    if(para_num!={self.args[arg['loc']-1]['name']}+para_loc){{
        break;
    }}

    {arg['type']} *{arg['name']}=g_malloc({self.args[arg['loc']-1]['name']}*sizeof({arg['type']}));
    for(int i=0;i<{self.args[arg['loc']-1]['name']};i++){{
        para_loc++; 
        {arg['name']}[i]=({arg['type']} )all_para[para_loc].data;
    }}
    """
                    free_thing.append(f"{arg['name']}")
                    out_file.write(in_ptr_str)
                else:
                if self.asyn:
                    #异步的情况上面处理缓冲区的时候已经处理完事了
                    in_ptr_str="\n"
                    pass
                else:
                    in_ptr_str = \
    f"""
    para_loc++;
    {arg['type']} {arg['name']}=({arg['type']} )all_para[para_loc].data;\n
    """
                    out_file.write(in_ptr_str)
            elif arg["ptr"] == 'out':
                out_ptr_str = \
    f"""/* Output ptr */
    para_loc++;
    {arg['type']} {arg['name']}=({arg['type']} )all_para[para_loc].data;
    mark_call_return(call,para_loc+1);
    \n
    """
                out_file.write(out_ptr_str)

        # Get call string
        #对于host调用来说，不需要有后面的后缀（比如with或者origin啥的），但是带with的会有不同处理方式，所以这里留下来
        # temp_name=self.name.split("_")[0]
        temp_name=self.name
        call_str = f"{temp_name}("
        for arg in self.args:
            call_str += f"{arg['name']}, "
        if len(self.args) > 0:
            call_str = call_str[:-2]  # Remove last ', '
        call_str += ');'

        # Generate run function string. Get return if needed.
        # No return value
        if self.ret == '':
            run_func_str = call_str + "\n"

        # Deal with return value
        # elif self.ret=='void*':
        #     pass
        else:
            # Ptr return value, they are all strings
            run_func_str = \
                f"""/* This function has a return value */
{self.ret} ret = {call_str}
set_call_return_val(call,ret, {sizeof(self.ret)});
\n
"""

        out_file.write(run_func_str)
        if len(free_thing) != 0:
            for thing in free_thing:
                out_file.write(f"g_free({thing});")

        # out_file.write("call->callback(call);\n")

        out_file.write("}\nbreak;\n\n\n")

    def generate_android_func(self, out_file):
        # Debug information
        out_file.write("\n")
        out_file.write(f"/* readline: \"{self.line}\" */\n")
        out_file.write(f"/* func name: \"{self.name}\" */\n")
        out_file.write(f"/* args: {self.args} */\n")
        out_file.write(f"/* ret: \"{self.ret}\" */\n")
        out_file.write(f"/* asyn: \"{self.asyn}\" */\n")
        out_file.write("\n")

        # Write definition string
        def_str = ''
        if self.ret != '':
            def_str = self.ret + ' '
        else:
            def_str = 'void '
        def_str += f"d_{self.name}(void *context, "
        for arg in self.args:
            if arg['ptr_ptr']:
                def_str += f"{arg['type']}const* {arg['name']}, "
            else:
                def_str += f"{arg['type']} {arg['name']}, "
        # if len(self.args) > 0:
        def_str = def_str[:-2]  # Remove last ', '
        all_include.write(def_str+');\n')
        def_str += ')\n{'
        out_file.write(def_str)

        # Protocol: FunID,
        #           data number,
        #           [data length, data] * data number

        para_num = ""

        # 根据是否同步定义一个用于传输数据的buf
        if self.asyn:
            buf_define_str = f"""char send_buf[24+{len(self.in_ptr_args)*16}];\n size_t send_buf_len=24;\n"""
            para_num = "1"
        elif self.PtrOfPtr:
            ptr_ptr_len = '0'
            for arg in self.in_ptr_args:
                if arg['ptr_ptr']:
                    ptr_ptr_len += f"""+{arg['ptr_len'].split("|")[0]}-1"""
            buf_define_str = f"""char *send_buf=(char *)malloc(4+4+16*({self.send_items_num}+{ptr_ptr_len}));\n size_t send_buf_len=4+4+16*({self.send_items_num}+{ptr_ptr_len});\n"""
            para_num = f"{self.send_items_num}+{ptr_ptr_len}"
        else:
            buf_define_str = f"""char send_buf[4+4+16*{self.send_items_num}];\n size_t send_buf_len=4+4+16*{self.send_items_num};\n"""
            para_num = f"{self.send_items_num}"

        # 新建一个用于保存int型或者异步情况下其他数据的数组
        new_buf_str = f"""unsigned long save_buf_len=0;\nint big_buf=0;\n"""
        new_buf_str += f"""save_buf_len+={int(self.get_non_ptr_arg_length())};\n"""
        if self.asyn:
            for arg in self.in_ptr_args:
                new_buf_str += f"""save_buf_len+={arg['ptr_len']};\n"""
            #这个是为了保证保存到缓冲区的数据不至于过大，假如过大，那所有的指针都不保存到缓冲区内
            new_buf_str += f"""if(save_buf_len>2048){{\n"""
            new_buf_str += f"""big_buf=1;\n"""
            new_buf_str += f"""send_buf_len+={len(self.in_ptr_args)*16};\n"""
            for arg in self.in_ptr_args:
                new_buf_str += f"""save_buf_len-={arg['ptr_len']};\n"""
            new_buf_str += "}"

        new_buf_str +=\
            f"""
char *save_buf=NULL;
char *ptr=NULL;
if(save_buf_len!=0){{
    save_buf=(char *)malloc(save_buf_len);
    if(save_buf==0||{para_num}>64){{
        //todo
        printf("need handle");
    }}

    ptr=save_buf;\n
"""

        out_file.write(buf_define_str)
        out_file.write(new_buf_str)

        action_str = ""
        # 在这个缓冲区内保存下所有的非指针数据，对于异步函数，还要在这个缓冲区内保存下实际的指针数据
        for arg in self.non_ptr_args:
            action_str += \
                f"""
*({arg['type']} *)ptr = {arg['name']};
ptr += {sizeof(arg['type'])};
"""
        
        if self.asyn and len(self.in_ptr_args)!=0:
            action_str += "if(big_buf==0){"
            for arg in self.in_ptr_args:
                action_str += \
                    f"""
memcpy(ptr,(char *){arg['name']},{arg['ptr_len']});
ptr+={arg['ptr_len']};
"""
            action_str+="\n}\n"    
        action_str+="\n}\n"
        # 正式把上面的缓冲区添加到要发送的数据区中
        action_str +=\
            f"""
ptr=send_buf;
*(int*)ptr=FUNID_{self.name};
ptr+=4;
if(big_buf==0){{
    *(int*)ptr=(int)({para_num});
}}else{{
    *(int*)ptr=(int)({para_num}+{len(self.in_ptr_args)});
}}
ptr+=4;
if(save_buf_len!=0){{
    *(unsigned long*)ptr=save_buf_len;
    ptr+=sizeof(unsigned long);
    *(unsigned long*)ptr=save_buf;
    ptr+=sizeof(unsigned long);
}}

"""

        # out_file.write(action_str)

        if self.asyn and len(self.in_ptr_args)!=0:
            action_str +="""if(big_buf==1){\n"""
            #这种情况下没有ptr_of_ptr的情况
            for arg in self.in_ptr_args:
                
                action_str +=\
                    f"""
*(unsigned long*)ptr={arg['ptr_len']};
ptr+=sizeof(unsigned long);
*(unsigned long*)ptr=(unsigned long){arg['name']};
ptr+=sizeof(unsigned long);
"""
            action_str += "\n}\n"

        # 同步模式对其他指针的操作
        if not self.asyn:

            for arg in self.args:

                if arg['ptr'] != "NA":
                    if not arg['ptr_ptr']:
                        action_str +=\
                            f"""
*(unsigned long*)ptr={arg['ptr_len']};
ptr+=sizeof(unsigned long);
*(unsigned long*)ptr=(unsigned long){arg['name']};
ptr+=sizeof(unsigned long);
"""

                    else:

                        len_temp = arg['ptr_len'].split("|")
                        ptr_ptr_len = len_temp[0]
                        ptr_len = len_temp[1]
                        action_str +=\
                            f"""
for(int i=0;i<{ptr_ptr_len};i++){{
    *(unsigned long*)ptr={ptr_len};
    ptr+=sizeof(unsigned long);
    *(unsigned long*)ptr=(unsigned long){arg['name']}[i];
    ptr+=sizeof(unsigned long);
}}

"""
        
            # 有返回值的情况，这种情况下不会是异步模式（当然，因为这是opengl，所以也可以改造为异步模式，例如直接返回某些"显然"的结果）
            if self.ret != "":
                action_str +=\
                    f"""
{self.ret} ret=0;
*(unsigned long*)ptr=sizeof({self.ret});
ptr+=sizeof(unsigned long);
*(unsigned long*)ptr=(unsigned long)&ret;
ptr+=sizeof(unsigned long);

"""

        out_file.write(action_str)

        out_file.write(f"send_to_host(context,send_buf,send_buf_len,save_buf,{int(self.asyn)});")

        if self.PtrOfPtr:
            out_file.write("free(send_buf);")


        # Return if needed(may require returning ptr)
        if self.ret != '':
            out_file.write("return ret;\n")

        # End of function
        out_file.write("}\n\n")

#
# End of class Func
#


define_id_async = 1
define_id_sync = 0

all_func_count = 0




qemu_out = open(f"qemu/all_out.c", "w+")
android_out = open(f"android/all_out.c", "w+")

qemu_define = open(f"qemu/define.h", "w+")
define_line = ""
# qemu_para_num=open(f"qemu/para_num.h","w+")
para_num_line = ""

android_out.write("""
#include "glad.h"
#include "glext.h"
#include "gl3.h"
#include "gl.h"

#include "define.h"

typedef khronos_int32_t  GLclampx;
\n""")

qemu_out.write(
    """

#include "glad.h"
#include "glext.h"
#include "gl3.h"
#include "gl.h"

#include "define.h"


void gl3_decode_invoke(Thread_Context *context,Direct_Express_Call *call)
{
    Render_Thread_Context *render_context=(Render_Thread_Context *)context;
    Opengl_Context *context=&(render_context->opengl_context);
    unsigned long fun_id=GET_FUN_ID(call->id);
    unsigned long is_async=FUN_IS_ASYNC(call->id);
    unsigned long need_speed=FUN_NEED_SPEED(call->id);
    Call_Para all_para[MAX_PARA_NUM];

    unsigned char num_buf[1024*4];
    
    switch (fun_id)
{\n""")
for filename in filenames:
    define_id = 0
    file_type = 0
    if filename[0] == "1":
        file_type = 1
        define_id = define_id_async
    else:
        file_type = 0
        define_id = define_id_sync
    input_file = open(f"in/{filename}.txt", "r", encoding="utf-8")
    # qemu_out = open(f"qemu/{filename}.h", "w+")



    qemu_out.write(f"\n\n/******* file '{filename}' *******/\n\n\n")
    android_out.write(f"\n\n/******* file '{filename}' *******/\n\n\n")

    func_count = 0
    for line in input_file:

        # Empty line means end of file
        if line.strip() == '' or line[0:2] == "//":
            continue
        # print(line)
        # Get func defination
        func = Func(line, file_type)
        func_count += 1
        all_func_count += 1

        # Work
        func.generate_qemu_case(qemu_out, define_id)
        func.generate_android_func(android_out)
        define_id += 2
    if filename[0] == "1":
        define_id_async = define_id
    else:
        define_id_sync = define_id

    # End for
    qemu_out.write(f"\n\n/******* end of file '{filename}', "
                   f"{func_count}/{all_func_count} functions*******/\n\n\n")
    android_out.write(f"\n\n/******* end of file '{filename}', "
                      f"{func_count}/{all_func_count} functions *******/\n\n\n")
    input_file.close()
    # qemu_out.close()

qemu_out.write(
    """    default:
        break;
    }
    call->callback(call, 0);
    return;
}\n""")


qemu_define.write("""
#ifndef GLV3_TRANS_H
#define GLV3_TRANS_H

#include "direct-express/direct_express_distribute.h"
#include "direct-express/express_device_common.h"\n
""")

qemu_define.write(define_line)
qemu_define.write("\n\n\n\n\n\n\n\n")
qemu_define.write(para_num_line)
qemu_define.write("""
void gl3_decode_invoke(Thread_Context *context,Direct_Express_Call *call);

#endif
""")

all_include.close()

qemu_out.close()
qemu_define.close()
# qemu_para_num.close()
