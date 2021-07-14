
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






def gen_android_sync(opengl_fun, out_file, is_copy):

    now_ret_num=0
    if not is_copy:
        now_ret_num=int(opengl_fun.ret!="")+len(opengl_fun.out_ptr_args)
    else:
        now_ret_num=int(len(opengl_fun.out_ptr_args)!=0 or opengl_fun.ret!="")

    if opengl_fun.PtrOfPtr:
        ptr_ptr_len = ''
        for arg in opengl_fun.in_ptr_args:
            if arg['ptr_ptr']:
                ptr_ptr_len += f"""+{arg['ptr_len'].split("|")[0]}"""
            else:
                ptr_ptr_len+= "+1"
        main_str = f"""
        unsigned char *send_buf=(unsigned char *)malloc(16+16*({int(opengl_fun.get_non_ptr_arg_length()!=0)+now_ret_num}{ptr_ptr_len}));
        size_t send_buf_len=16+16*({int(opengl_fun.get_non_ptr_arg_length()!=0)+now_ret_num}{ptr_ptr_len});
        size_t para_num={int(opengl_fun.get_non_ptr_arg_length()!=0)+now_ret_num}{ptr_ptr_len};
        """
    else:

        main_str=f"""
        unsigned char send_buf[16+{int(opengl_fun.get_non_ptr_arg_length()!=0)+len(opengl_fun.in_ptr_args)+now_ret_num}*16];
        size_t send_buf_len=16+{int(opengl_fun.get_non_ptr_arg_length()!=0)+len(opengl_fun.in_ptr_args)+now_ret_num}*16;
        size_t para_num={int(opengl_fun.get_non_ptr_arg_length()!=0)+len(opengl_fun.in_ptr_args)+now_ret_num};
        """
    
    opengl_fun.send_items_num=int(opengl_fun.get_non_ptr_arg_length()!=0)+len(opengl_fun.in_ptr_args)+now_ret_num
    



    main_str+="unsigned char *ptr=NULL;\n"

    if int(opengl_fun.get_non_ptr_arg_length())<1000 and int(opengl_fun.get_non_ptr_arg_length())!=0:
        main_str+=f"""
        
        uint64_t save_buf_len={int(opengl_fun.get_non_ptr_arg_length())};
        unsigned char *save_buf;
        """
        main_str+=f"""
        unsigned char local_save_buf[{int(opengl_fun.get_non_ptr_arg_length())}];
        save_buf=local_save_buf;
        ptr=save_buf;
        """
    elif int(opengl_fun.get_non_ptr_arg_length())>=1000:
        main_str+=f"""

        uint64_t save_buf_len={int(opengl_fun.get_non_ptr_arg_length())};
        unsigned char *save_buf;
        save_buf=(unsigned char *)malloc(save_buf_len);   
        ptr=save_buf;        
        """
    
    for arg in opengl_fun.non_ptr_args:
        main_str += f"""

        *({arg['type']} *)ptr = {arg['name']};
        ptr += {sizeof(arg['type'])};
        """

    main_str+=f"""

    ptr=send_buf;
    *(uint64_t*)ptr=FUNID_{opengl_fun.name};
    ptr+=sizeof(uint64_t);
    """
    if int(opengl_fun.get_non_ptr_arg_length())!=0:
        main_str+=f"""
        *(uint64_t*)ptr=(uint64_t)para_num;
        ptr+=sizeof(uint64_t);

        *(uint64_t*)ptr=(uint64_t)save_buf_len;
        ptr+=sizeof(uint64_t);
        *(uint64_t*)ptr=(uint64_t)save_buf;
        ptr+=sizeof(uint64_t);

        """
    else:
        main_str+=f"""
        *(uint64_t*)ptr=(uint64_t)para_num;
        ptr+=sizeof(uint64_t);

    """

    for arg in opengl_fun.in_ptr_args:
        if arg['ptr_ptr']:
            main_str +=f"""

            for(int i=0;i<{arg['ptr_len'].split("|")[0]};i++){{
                *(uint64_t*)ptr=(uint64_t){arg['ptr_len'].split("|")[1]};
                ptr+=sizeof(uint64_t);
                *(uint64_t*)ptr=(uint64_t){arg['name']}[i];
                ptr+=sizeof(uint64_t);
            }}
            
            """
        else:
            main_str +=f"""

            *(uint64_t*)ptr={arg['ptr_len']};
            ptr+=sizeof(uint64_t);
            *(uint64_t*)ptr=(uint64_t){arg['name']};
            ptr+=sizeof(uint64_t);

            """

    if len(opengl_fun.out_ptr_args)!=0 or opengl_fun.ret!="":
        if not is_copy:
            for arg in opengl_fun.out_ptr_args:
                main_str +=f"""

                *(uint64_t*)ptr=(uint64_t){arg['ptr_len']};
                ptr+=sizeof(uint64_t);
                *(uint64_t*)ptr=(uint64_t){arg['name']};
                ptr+=sizeof(uint64_t);
                """
            

            if opengl_fun.ret != "":
                main_str +=  f"""

                {opengl_fun.ret} ret=0;
                *(uint64_t*)ptr=(uint64_t)sizeof({opengl_fun.ret});
                ptr+=sizeof(uint64_t);
                *(uint64_t*)ptr=(uint64_t)&ret;
                ptr+=sizeof(uint64_t);

            """
        else:
            main_str +=f"""
                unsigned char * out_buf;
                unsigned char * out_buf_ptr;
                uint64_t out_buf_len=0;
                unsigned char out_buf_local[MAX_OUT_BUF_LEN];
            """
            for arg in opengl_fun.out_ptr_args:
                main_str +=f"""

                uint64_t {arg['name']}_len=(uint64_t){arg['ptr_len']};
                out_buf_len+={arg['name']}_len;
                """
            if opengl_fun.ret != "":
                main_str+=f"""
                    out_buf_len+=sizeof({opengl_fun.ret});
                """
            
            main_str +=  f"""
                if(out_buf_len>MAX_OUT_BUF_LEN){{
                    out_buf=(unsigned char *)malloc(out_buf_len); 
                }}else{{
                    out_buf=out_buf_local;
                }}
            """
            main_str +=f"""

                *(uint64_t*)ptr=(uint64_t)out_buf_len;
                ptr+=sizeof(uint64_t);
                *(uint64_t*)ptr=(uint64_t)out_buf;
                ptr+=sizeof(uint64_t);
            """

    main_str+=f"""

        send_to_host(context,send_buf,send_buf_len,1);


    """
    if int(opengl_fun.get_non_ptr_arg_length())>=1000:
        main_str+="free(save_buf);\n"

    if opengl_fun.PtrOfPtr:
        main_str+="free(send_buf);\n"

    if is_copy and (len(opengl_fun.out_ptr_args)!=0 or opengl_fun.ret!=""):
        main_str+="out_buf_ptr=out_buf;"
        for arg in opengl_fun.out_ptr_args:
            main_str +=f"""
                if({arg['name']}!=NULL){{

                    memcpy({arg['name']},({arg['type']})out_buf_ptr,{arg['name']}_len);
                }}
                out_buf_ptr+={arg['name']}_len;
            """
        if opengl_fun.ret != "":
            main_str+=f"""
                {opengl_fun.ret} ret=*({opengl_fun.ret} *)out_buf_ptr;
            """
        main_str+="""
            if(out_buf_len>MAX_OUT_BUF_LEN){{
                free(out_buf); 
            }}
        """

    # Return if needed(may require returning ptr)
    if opengl_fun.ret != '':
        main_str+=f"""
            return ret;\n
        """


    out_file.write(main_str)



    
def gen_android_asyn_copy(opengl_fun, out_file):
    #@todo 异步调用保存数据，减少陷入内核次数

    main_str=f"""
    unsigned char send_buf[16+16*{int(opengl_fun.get_non_ptr_arg_length()!=0)}];
    size_t send_buf_len=16+16*{int(opengl_fun.get_non_ptr_arg_length()!=0)};
    uint64_t save_buf_len={int(opengl_fun.get_non_ptr_arg_length())};
    unsigned char local_save_buf[4096];
    unsigned char *save_buf;
    """
    opengl_fun.send_items_num=int(opengl_fun.get_non_ptr_arg_length()!=0)
    for arg in opengl_fun.in_ptr_args:
        if arg['ptr_ptr']:
                ptr_ptr_size=arg['ptr_len'].split("|")[0]
                ptr_size=arg['ptr_len'].split("|")[1]
                main_str+=f"""
                    size_t *{arg['name']}_len=(size_t *)malloc({ptr_ptr_size}*sizeof(size_t));
                    for(int i=0;i<{ptr_ptr_size};i++){{
                        {arg['name']}_len[i]={ptr_size};
                        save_buf_len+={arg['name']}_len[i];
                    }}
                """
        else:

            main_str += f"""
                size_t {arg['name']}_len={arg['ptr_len']};
                save_buf_len+={arg['name']}_len;
            """

    main_str+=f"""

        if(save_buf_len>MAX_OUT_BUF_LEN){{
            save_buf=(unsigned char *)malloc(save_buf_len);
        }}else{{
            save_buf=local_save_buf;
        }}

        unsigned char *ptr=save_buf; 

    """
    
    for arg in opengl_fun.non_ptr_args:
        main_str += f"""

        *({arg['type']} *)ptr = {arg['name']};
        ptr += {sizeof(arg['type'])};
        """
    for arg in opengl_fun.in_ptr_args:
        if arg['ptr_ptr']:
            ptr_ptr_size=arg['ptr_len'].split("|")[0]
            ptr_size=arg['ptr_len'].split("|")[1]
            main_str+=f"""
                for(int i=0;i<{ptr_ptr_size};i++){{
                    memcpy(ptr,(unsigned char *){arg['name']}[i],{arg['name']}_len[i]);
                    ptr+={arg['name']}_len[i];
                }}
            """
        else:
            main_str += f"""

            memcpy(ptr,(unsigned char *){arg['name']},{arg['name']}_len);
            ptr+={arg['name']}_len;
            """
    main_str+=f"""

    ptr=send_buf;

    *(uint64_t*)ptr=FUNID_{opengl_fun.name};
    ptr+=sizeof(uint64_t);
    """
    if int(opengl_fun.get_non_ptr_arg_length())==0:
        main_str+=f"""
            
            *(uint64_t*)ptr=0;
            ptr+=sizeof(uint64_t);

        """
    else:
        main_str+=f"""
            *(uint64_t*)ptr=1;
            ptr+=sizeof(uint64_t);

            *(uint64_t*)ptr=(uint64_t)save_buf_len;
            ptr+=sizeof(uint64_t);
            *(uint64_t*)ptr=(uint64_t)save_buf;
            ptr+=sizeof(uint64_t);
        """
    if opengl_fun.name.find("Flush")!=-1 or opengl_fun.name.find("Finish")!=-1 or opengl_fun.name.find("Swap")!=-1 or opengl_fun.name.find("Draw")!=-1:
        main_str+=f"""
            send_to_host(context,send_buf,send_buf_len,1);
            """
    else:
        main_str+=f"""
            send_to_host(context,send_buf,send_buf_len,0);
            """
    main_str+=f"""
        if(save_buf_len>MAX_OUT_BUF_LEN){{
            free(save_buf);
        }}

    """

    if opengl_fun.ret!="":
        if opengl_fun.ret=='GLboolean':
            main_str+="return GL_TRUE;"
        else:
            print("error need handle")
            print(opengl_fun.name)

    out_file.write(main_str)


def gen_android_asyn_no_copy(opengl_fun, out_file):
    main_str=f"""
    unsigned char send_buf[16+({int(opengl_fun.get_non_ptr_arg_length()!=0)+len(opengl_fun.in_ptr_args)})*16];
    size_t send_buf_len=16+({int(opengl_fun.get_non_ptr_arg_length()!=0)+len(opengl_fun.in_ptr_args)})*16;
    """

    opengl_fun.send_items_num=int(opengl_fun.get_non_ptr_arg_length()!=0)+len(opengl_fun.in_ptr_args)
    main_str+="unsigned char *ptr=NULL;\n"


    if int(opengl_fun.get_non_ptr_arg_length())<1000 and int(opengl_fun.get_non_ptr_arg_length())!=0:
        main_str+=f"""

        uint64_t save_buf_len={int(opengl_fun.get_non_ptr_arg_length())};
        unsigned char *save_buf;
        """
        main_str+=f"""
        unsigned char local_save_buf[{int(opengl_fun.get_non_ptr_arg_length())}];
        save_buf=local_save_buf;
        ptr=save_buf;
        """
    elif int(opengl_fun.get_non_ptr_arg_length())>=1000:
        main_str+=f"""

        uint64_t save_buf_len={int(opengl_fun.get_non_ptr_arg_length())};
        unsigned char *save_buf;
        save_buf=(unsigned char *)malloc(save_buf_len);   
        ptr=save_buf;        
        """
    
    for arg in opengl_fun.non_ptr_args:
        main_str += f"""

        *({arg['type']} *)ptr = {arg['name']};
        ptr += {sizeof(arg['type'])};
        """

    main_str+=f"""

    ptr=send_buf;

    *(uint64_t*)ptr=FUNID_{opengl_fun.name};
    ptr+=sizeof(uint64_t);
    """

    # main_str+=f"""
    #     *(uint64_t*)ptr={len(opengl_fun.in_ptr_args)+len(opengl_fun.out_ptr_args)};
    #     ptr+=sizeof(uint64_t);
    #     """
    if int(opengl_fun.get_non_ptr_arg_length())==0:
        main_str+=f"""
        *(uint64_t*)ptr={len(opengl_fun.in_ptr_args)};
        ptr+=sizeof(uint64_t);
        """
    else:
        main_str+=f"""
        *(uint64_t*)ptr=(uint64_t)(1+{len(opengl_fun.in_ptr_args)});
        ptr+=sizeof(uint64_t);

        *(uint64_t*)ptr=(uint64_t)save_buf_len;
        ptr+=sizeof(uint64_t);
        *(uint64_t*)ptr=(uint64_t)save_buf;
        ptr+=sizeof(uint64_t);
        """

    for arg in opengl_fun.in_ptr_args:

        main_str +=f"""

            *(uint64_t*)ptr=(uint64_t){arg['ptr_len']};
            ptr+=sizeof(uint64_t);
            *(uint64_t*)ptr=(uint64_t){arg['name']};
            ptr+=sizeof(uint64_t);
            """
    
    main_str+=f"""

        send_to_host(context,send_buf,send_buf_len,1);

    """
    # if int(opengl_fun.get_non_ptr_arg_length())>=1000:
    #     main_str+="free(save_buf);"



    out_file.write(main_str)
