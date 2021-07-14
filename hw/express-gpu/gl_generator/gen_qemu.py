
import sys

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





def sizeof(arg_type):
    return int(sizeof_dic[arg_type] / sizeof_dic['GLbyte'])


def gen_qemu_asyn_copy(opengl_fun, out_file,is_special):
    non_ptr_args_length = f"{int(opengl_fun.get_non_ptr_arg_length())}"
    if non_ptr_args_length != "0" or len(opengl_fun.args)!=0:

        main_str = f"""
            size_t temp_len=0;
            unsigned char *temp=NULL;

            temp_len=all_para[0].data_len;
            if(temp_len< {non_ptr_args_length} * {sizeof('GLbyte')}){{
                break;
            }}
            
            int null_flag=0;
            temp=get_direct_ptr(all_para[0].data,&null_flag);
            if(temp==NULL){{
                if(temp_len!=0 &&null_flag==0){{
                    temp=no_ptr_buf;
                    guest_write(all_para[0].data,temp,0,all_para[0].data_len);

                }}else{{
                    break;
                }}
            }}

            unsigned int temp_loc=0;
        """
    else:
        main_str=""

    for arg in opengl_fun.args:
        # Read non-ptr variable
        if arg['ptr'] == 'NA':
            main_str += f"""

                {arg['name']} = *({arg['type']} *)(temp+temp_loc);
                temp_loc+={sizeof(arg['type'])};
            """

    for arg in opengl_fun.args:
        # Read ptr variable
        if arg['ptr'] == 'in':
            if not arg['ptr_ptr']:
                main_str += f"""
                    {arg['type']} {arg['name']}=({arg['type']})(temp+temp_loc);
                    temp_loc+={arg['ptr_len']};
                """
            else:
                ptr_num = arg['ptr_len'].split("|")[0]
                ptr_len=arg['ptr_len'].split("|")[1]
                main_str += f"""  
                    {arg['type'].strip('const')} *{arg['name']}=g_malloc({ptr_num}*sizeof({arg['type']}));

                    for(int i=0;i<{ptr_num};i++){{
                        {arg['name']}[i] = ({arg['type'].strip('const')})(temp+temp_loc);
                        temp_loc+={ptr_len};
                    }}
                """
                
    if non_ptr_args_length != "0" or len(opengl_fun.args)!=0:
        main_str += \
        f"""/* Check length */
            if(temp_len < temp_loc){{
                break;
            }}\n
            """
    
    # if opengl_fun.name.find("_") == -1 or opengl_fun.name.find("_v") != -1:
    if not is_special and (opengl_fun.name.find("_") == -1 or opengl_fun.name.find("_v") != -1):
        call_str = f"{opengl_fun.name}("
    else:
        call_str = f"d_{opengl_fun.name}(opengl_context,"

    for arg in opengl_fun.args:
        call_str += f"{arg['name']}, "
    if len(opengl_fun.args) > 0:
        call_str = call_str[:-2]  # Remove last ', '
    call_str += ');\n'

    main_str+="\n\n"+call_str

    for arg in opengl_fun.args:
        # Read ptr variable
        if arg['ptr'] == 'in':
            if arg['ptr_ptr']:
                main_str += f"""  
                    g_free({arg['name']});
                """
    
    out_file.write(main_str)


buffer_id()

def gen_qemu_asyn_no_copy(opengl_fun, out_file):
    non_ptr_args_length = f"{int(opengl_fun.get_non_ptr_arg_length())}"
    loc = 0

    if non_ptr_args_length != "0":

        main_str = f"""
            size_t temp_len=0;
            unsigned char *temp=NULL;

            temp_len=all_para[0].data_len;
            if(temp_len< {non_ptr_args_length} * {sizeof('GLbyte')}){{
                break;
            }}

            int null_flag=0;
            temp=get_direct_ptr(all_para[0].data,&null_flag);
            if(temp==NULL){{
                if(temp_len!=0 && null_flag==0){{
                    temp=no_ptr_buf;
                    guest_write(all_para[0].data,temp,0,all_para[0].data_len);
                }}else{{
                    break;
                }}
            }}
            

            unsigned int temp_loc=0;
        """
        loc+=1
    else:
        main_str=""

    for arg in opengl_fun.args:
        # Read non-ptr variable
        if arg['ptr'] == 'NA':
            main_str += f"""

                {arg['name']} = *({arg['type']} *)(temp+temp_loc);
                temp_loc+={sizeof(arg['type'])};
            """

    for arg in opengl_fun.args:
        # Read ptr variable
        if arg['ptr'] == 'in':

            main_str += f"""

                void *{arg['name']}=all_para[{loc}].data;
            """
            loc += 1
    for arg in opengl_fun.args:
        # Read ptr variable
        if arg['ptr'] == 'out':

            main_str += f"""

                void *{arg['name']}=all_para[{loc}].data;
            """
            loc += 1


    call_str = f"d_{opengl_fun.name}(opengl_context,"
    for arg in opengl_fun.args:
        call_str += f"{arg['name']}, "
    if len(opengl_fun.args) > 0:
        call_str = call_str[:-2]  # Remove last ', '
    call_str += ');\n'
    
    out_file.write(main_str+"\n\n"+call_str)





def gen_qemu_sync(opengl_fun, out_file,is_copy):
    non_ptr_args_length = f"{int(opengl_fun.get_non_ptr_arg_length())}"
    loc=0
    if non_ptr_args_length != "0":
        main_str = f"""
            size_t temp_len=0;
            unsigned char *temp=NULL;

            temp_len=all_para[0].data_len;
            if(temp_len< {non_ptr_args_length} * {sizeof('GLbyte')}){{
                break;
            }}

            int null_flag=0;
            temp=get_direct_ptr(all_para[0].data,&null_flag);
            if(temp==NULL){{
                if(temp_len!=0 && null_flag==0){{
                    temp=no_ptr_buf;
                    guest_write(all_para[0].data,temp,0,all_para[0].data_len);
                }}else{{
                    break;
                }}
            }}

        """
        main_str += "unsigned int temp_loc=0;"
        loc += 1

    else:
        main_str = f""""""

    for arg in opengl_fun.args:
        # Read non-ptr variable
        if arg['ptr'] == 'NA':
            main_str += f"""

                {arg['name']} = *({arg['type']} *)(temp+temp_loc);
                temp_loc+={sizeof(arg['type'])};
            """

    in_ptr_get = 0
    ptr_num = ""

    ptr_loc=""
    for arg in opengl_fun.args:
        # Read ptr variable
        if arg['ptr'] == 'in':
            if not is_copy:
            # if opengl_fun.name.find("_") != -1 and opengl_fun.name.find("_v") == -1 and opengl_fun.name.find("_origin") == -1:
                #arg['type'] == 'const void*' or
                # 这种情况直接输入就行了
                main_str += f"""
                    void *{arg['name']}=all_para[{loc}{ptr_loc}].data;
                """
                loc += 1
            elif not arg['ptr_ptr']:
                in_ptr_get = 1
                main_str += f"""
                    int {arg['name']}_flag=0;
                    int {arg['name']}_null_flag=0;
                    {arg['type'].strip('const')} {arg['name']}=get_direct_ptr(all_para[{loc}{ptr_loc}].data, &{arg['name']}_null_flag);

                    if({arg['name']}==NULL && {arg['name']}_null_flag==0){{
                        {arg['name']}=g_malloc(all_para[{loc}{ptr_loc}].data_len);
                        guest_write(all_para[{loc}{ptr_loc}].data,{arg['name']},0,all_para[{loc}{ptr_loc}].data_len);
                        
                        {arg['name']}_flag=1;
                    }}else{{
                        {arg['name']}_flag=0;
                    }}
                """
                loc += 1
            else:
                # if arg['ptr_ptr']:
                in_ptr_get = 1
                ptr_num = arg['ptr_len'].split("|")[0]
                main_str += f"""  
                    {arg['type'].strip('const')} *{arg['name']}=g_malloc({ptr_num}*sizeof({arg['type']}));
                    int *{arg['name']}_flag=g_malloc({ptr_num}*sizeof(int *));
                    int {arg['name']}_null_flag;

                    for(int i=0;i<{ptr_num};i++){{
                        {arg['name']}[i]=get_direct_ptr(all_para[{loc}{ptr_loc}+i].data, &{arg['name']}_null_flag);

                        if({arg['name']}[i]==NULL && {arg['name']}_null_flag==0){{
                            {arg['name']}[i]=g_malloc(all_para[{loc}{ptr_loc}+i].data_len);
                            guest_write(all_para[{loc}{ptr_loc}+i].data,{arg['name']}[i],0,all_para[{loc}{ptr_loc}+i].data_len);
                            
                            {arg['name']}_flag[i]=1;
                        }}else{{
                            {arg['name']}_flag[i]=0;
                        }}

                    }}
                """
                ptr_loc+=f"+{ptr_num}"
            

    if len(opengl_fun.out_ptr_args) != 0 or opengl_fun.ret != "":
        if is_copy:
            main_str+=f"int out_buf_len=all_para[{loc}{ptr_loc}].data_len;\n"
            main_str += f"""

                unsigned char *ret_buf=NULL;

                if(out_buf_len>MAX_OUT_BUF_LEN){{
                    ret_buf=g_malloc(out_buf_len);
                }}else{{
                    ret_buf=ret_local_buf;
                }}
                int out_buf_loc=0;
            
            """
            for arg in opengl_fun.args:
                # Read ptr variable
                if arg['ptr'] == 'out':
                    if arg['ptr_len'].find('gl_get_program_uniform_size')!=-1 or arg['ptr_len'].find('gl_get_uniform_block_para_size')!=-1:
                            main_str += f"""

                        {arg['type']} {arg['name']}=({arg['type']})(ret_buf+out_buf_loc);
                        //out_buf_loc+={arg['ptr_len']};
                    """
                    else:
                        main_str += f"""

                        {arg['type']} {arg['name']}=({arg['type']})(ret_buf+out_buf_loc);
                        out_buf_loc+={arg['ptr_len']};
                    """
            if opengl_fun.ret!="":
                main_str += f"""
                    {opengl_fun.ret} *ret_ptr=({opengl_fun.ret} *)(ret_buf+out_buf_loc);
                    out_buf_loc+=sizeof({opengl_fun.ret});
                """
            main_str+=f"""
                if(out_buf_loc > out_buf_len){{
                    if(out_buf_len>MAX_OUT_BUF_LEN){{
                        g_free(ret_buf);
                    }}
                    break;
                }}
            """
        elif not is_copy:
            for arg in opengl_fun.args:
                # Read ptr variable
                if arg['ptr'] == 'out':
                    main_str += f"""
                        {arg['type']} {arg['name']}=all_para[{loc}{ptr_loc}].data;
                    """
                    loc+=1

    # if opengl_fun.name.find("_") == -1 or opengl_fun.name.find("_v") != -1:
    if is_copy:
        call_str = f"{opengl_fun.name}("
    else:
        call_str = f"d_{opengl_fun.name}(opengl_context,"

    for arg in opengl_fun.args:
        call_str += f"{arg['name']}, "
    if len(opengl_fun.args) > 0:
        call_str = call_str[:-2]  # Remove last ', '
    call_str += ');\n'

    if opengl_fun.ret!="":
        call_str=f"{opengl_fun.ret} ret = "+call_str

    out_file.write(main_str+"\n\n"+call_str)

    main_str = ""
    if is_copy:
        if opengl_fun.ret!="":
            main_str+="*ret_ptr=ret;\n"

        if len(opengl_fun.out_ptr_args) != 0 or opengl_fun.ret!="":
            main_str += f"""
                guest_read(all_para[{loc}{ptr_loc}].data,ret_buf,0,out_buf_len);
            """
    elif not is_copy:
        if opengl_fun.ret!="":
            main_str+=f"""
                guest_read(all_para[{loc}{ptr_loc}].data,&ret,0,sizeof({opengl_fun.ret}));
            """

    if in_ptr_get:
        for arg in opengl_fun.args:
            # Read ptr variable
            if arg['ptr'] == 'in':
                if arg['type'] == 'const void*':
                    pass
                elif not arg['ptr_ptr']:
                    main_str += f"""

                        if({arg['name']}_flag==1){{
                            g_free({arg['name']});
                        }}
                    """
                else:
                    main_str += f"""

                        for(int i=0;i<{opengl_fun.args[arg['loc']-1]['name']};i++){{
                            if({arg['name']}_flag[i]==1){{
                                g_free({arg['name']}[i]);
                            }}
                        }}

                        g_free({arg['name']});
                        g_free({arg['name']}_flag);
                    """

    out_file.write(main_str)

    if len(opengl_fun.out_ptr_args) != 0 or opengl_fun.ret != "":
        if is_copy:
            out_file.write("\n\nif(out_buf_len>MAX_OUT_BUF_LEN){g_free(ret_buf);}")

