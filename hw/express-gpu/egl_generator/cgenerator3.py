#!/bin/python3
# -*- coding: UTF-8 -*-

import sys

# gl_pixel_data_size(width,height,format,type)
filenames = [
    "1-1-1",  # 同步且数据需要复制出来
    "1-1-2",  # 同步且数据不需要复制出来，直接传输scatter内存
    "1-2",  # 同步，且涉及到中间状态，可以不传输到host端
    "2-1-1",  # 异步，且数据都是小数据，之后交给host端时可以直接复制出来
    "2-1-2",  # 异步，且数据都是不定长数据，host端实现了相应的不复制处理scatter内存的模式
    "2-2",  # 异步，会有返回值需要同步的情况，但是这些东西主要涉及到资源的申请，可以以提前申请来规避，目前可以先以同步来实现
]


# 64-bit target?
target64bit = True

ptrbits = 64 if target64bit else 32
sizeof_dic = {
    'EGLBoolean': 32,     'EGLenum': 32,           'EGLTime': 64,
    'EGLint':32,
    'EGLConfig': ptrbits, 'EGLSurface': ptrbits, 'EGLContext': ptrbits,
    'EGLDisplay': ptrbits, 'EGLClientBuffer':ptrbits,'EGLSync':ptrbits,
    'EGLAttrib':ptrbits, 'EGLImage':ptrbits,'EGLClientBuffer':ptrbits,
    'GLbyte':8,

    # GLchar is not defined in documents, but is defined as 'char' in header
    # files
}


all_include = open(f"all_egl.h", "w+")
all_include.write("""
#ifndef ALL_EGL_H
#define ALL_EGL_H

#include <EGL/egl.h>
""")


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

        self.type = file_type

        self.args_num = 0
        self.send_items_num = 0

        self.PtrOfPtr = False

        self.read_def()

    def read_def(self):

        # print(self.line)
        temp=self.line.split("@")
        if len(temp)>1:
            self.check_error=temp[1]
        else:
            self.check_error=""


        temp = temp[0].split()

        # Check if there is a return value
        offset = 0
        if temp[0][:2] != 'egl':
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
            # self.send_items_num = len(self.in_ptr_args)
            # self.send_items_num += len(self.out_ptr_args)
            # if len(self.non_ptr_args) != 0:
            #     self.send_items_num += 1

            # if self.ret != "":
            #     self.send_items_num += 1

    def get_non_ptr_arg_length(self):
        # Get size of non ptr arguments
        non_ptr_arg_length = 0
        for arg in self.non_ptr_args:
            non_ptr_arg_length += sizeof(arg['type'])
        return non_ptr_arg_length






    def gen_qemu_asyn_copy(self, out_file):
        non_ptr_args_length = f"{int(self.get_non_ptr_arg_length())}"
        if non_ptr_args_length != "0" or len(self.args)!=0:

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

        for arg in self.args:
            # Read non-ptr variable
            if arg['ptr'] == 'NA':
                main_str += f"""

                    {arg['name']} = *({arg['type']} *)(temp+temp_loc);
                    temp_loc+={sizeof(arg['type'])};
                """

        for arg in self.args:
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
                    
        if non_ptr_args_length != "0" or len(self.args)!=0:
            main_str += \
            f"""/* Check length */
                if(temp_len < temp_loc){{
                    break;
                }}\n
                """
        if self.name.find("_") == -1 or self.name.find("_v") != -1:
            call_str = f"{self.name}("
        else:
            call_str = f"d_{self.name}(render_context,"

        for arg in self.args:
            call_str += f"{arg['name']}, "
        if len(self.args) > 0:
            call_str = call_str[:-2]  # Remove last ', '
        call_str += ');\n'

        for arg in self.args:
            # Read ptr variable
            if arg['ptr'] == 'in':
                if arg['ptr_ptr']:
                    main_str += f"""  
                        g_free({arg['name']});
                    """
        
        out_file.write(main_str+"\n\n"+call_str)




    def gen_qemu_asyn_no_copy(self, out_file):
        non_ptr_args_length = f"{int(self.get_non_ptr_arg_length())}"
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

        for arg in self.args:
            # Read non-ptr variable
            if arg['ptr'] == 'NA':
                main_str += f"""

                    {arg['name']} = *({arg['type']} *)(temp+temp_loc);
                    temp_loc+={sizeof(arg['type'])};
                """

        for arg in self.args:
            # Read ptr variable
            if arg['ptr'] == 'in':

                main_str += f"""

                    void *{arg['name']}=all_para[{loc}].data;
                """
                loc += 1
        for arg in self.args:
            # Read ptr variable
            if arg['ptr'] == 'out':

                main_str += f"""

                    void *{arg['name']}=all_para[{loc}].data;
                """
                loc += 1


        call_str = f"d_{self.name}(render_context,"
        for arg in self.args:
            call_str += f"{arg['name']}, "
        if len(self.args) > 0:
            call_str = call_str[:-2]  # Remove last ', '
        call_str += ');\n'
        
        out_file.write(main_str+"\n\n"+call_str)





    def gen_qemu_sync(self, out_file):
        non_ptr_args_length = f"{int(self.get_non_ptr_arg_length())}"
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

        for arg in self.args:
            # Read non-ptr variable
            if arg['ptr'] == 'NA':
                main_str += f"""

                    {arg['name']} = *({arg['type']} *)(temp+temp_loc);
                    temp_loc+={sizeof(arg['type'])};
                """

        in_ptr_get = 0
        ptr_num = ""

        ptr_loc=""
        for arg in self.args:
            # Read ptr variable
            if arg['ptr'] == 'in':
                if self.type == 1:
                # if self.name.find("_") != -1 and self.name.find("_v") == -1 and self.name.find("_origin") == -1:
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
                

        if len(self.out_ptr_args) != 0 or self.ret != "":
            if self.type==0 or self.type==2 or self.type==5:
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
                for arg in self.args:
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
                if self.ret!="":
                    main_str += f"""
                        {self.ret} *ret_ptr=({self.ret} *)(ret_buf+out_buf_loc);
                        out_buf_loc+=sizeof({self.ret});
                    """
                main_str+=f"""
                    if(out_buf_loc > out_buf_len){{
                        if(out_buf_len>MAX_OUT_BUF_LEN){{
                            g_free(ret_buf);
                        }}
                        break;
                    }}
                """
            elif self.type==1:
                for arg in self.args:
                    # Read ptr variable
                    if arg['ptr'] == 'out':
                        main_str += f"""
                            {arg['type']} {arg['name']}=all_para[{loc}{ptr_loc}].data;
                        """
                        loc+=1

        if self.name.find("_") == -1 or self.name.find("_v") != -1:
            call_str = f"{self.name}("
        else:
            call_str = f"d_{self.name}(render_context,"

        for arg in self.args:
            call_str += f"{arg['name']}, "
        if len(self.args) > 0:
            call_str = call_str[:-2]  # Remove last ', '
        call_str += ');\n'

        if self.ret!="":
            call_str=f"{self.ret} ret = "+call_str

        out_file.write(main_str+"\n\n"+call_str)

        main_str = ""
        if self.type==0 or self.type==2  or self.type==5:
            if self.ret!="":
                main_str+="*ret_ptr=ret;\n"

            if len(self.out_ptr_args) != 0 or self.ret!="":
                main_str += f"""
                    guest_read(all_para[{loc}{ptr_loc}].data,ret_buf,0,out_buf_len);
                """
        elif self.type==1:
            if self.ret!="":
                main_str+=f"""
                    guest_read(all_para[{loc}{ptr_loc}].data,&ret,0,sizeof({self.ret}));
                """

        if in_ptr_get:
            for arg in self.args:
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

                            for(int i=0;i<{self.args[arg['loc']-1]['name']};i++){{
                                if({arg['name']}_flag[i]==1){{
                                    g_free({arg['name']}[i]);
                                }}
                            }}

                            g_free({arg['name']});
                            g_free({arg['name']}_flag);
                        """

        out_file.write(main_str)

        if len(self.out_ptr_args) != 0 or self.ret != "":
            if self.type!=1:
                out_file.write("\n\nif(out_buf_len>MAX_OUT_BUF_LEN){g_free(ret_buf);}")

    def generate_qemu_case(self, out_file, define_id):
        global qemu_define_line
        global android_define_line
        global para_num_line

        flag_str=""

        # 0 1 2 5
        if self.type!=3 and self.type!=4:
            flag_str+="+(((unsigned long long)0x1)<<24u)"

        
        qemu_define_line += f"#define FUNID_{self.name} ((EXPRESS_GPU_FUN_ID<<32u){flag_str}+{define_id})\n"
        android_define_line += f"\n#define FUNID_{self.name} ((EXPRESS_GPU_FUN_ID<<32u){flag_str}+{define_id})\n"

        # para_num = len(self.in_ptr_args)+len(self.out_ptr_args)
        # if len(self.non_ptr_args) != 0:
        #     para_num += 1
        # if self.type == 0 or self.type==2 or self.type==5:
        #     if len(self.out_ptr_args)!=0:
        #         self.send_items_num-=len(self.out_ptr_args)
        #         self.send_items_num+=1
        #         if self.ret!="":
        #             self.send_items_num-=1
        # if self.type == 3:
        #     if len(self.args)!=0:    
        #         self.send_items_num = 1




        # Case
        out_file.write(f"case FUNID_{self.name}:\n\n{{\n")
        # Debug information
        out_file.write("\n")
        out_file.write(f"/* readline: \"{self.line}\" */\n")
        out_file.write(f"/* func name: \"{self.name}\" */\n")
        out_file.write(f"/* args: {self.args} */\n")
        out_file.write(f"/* ret: \"{self.ret}\" */\n")
        out_file.write(f"/* type: \"{self.type}\" */\n")
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


"""
        out_file.write(header_str)
        if self.type == 0 or self.type == 1 or self.type==2:
            # 同步，0为复制出数据，1为不复制出数据
            self.gen_qemu_sync(out_file)
        
        elif self.type==3:
            # 异步，数据都是复制到内核里的，假如内存分散，也会复制聚合到一起
            self.gen_qemu_asyn_copy(out_file)
        elif self.type==4:
            # 异步，但是数据就算再内存中分散也不会复制到一起，guest端内存会直接呈递给后面的程序
            self.gen_qemu_asyn_no_copy(out_file)
        elif self.type==5:
            # 对于qemu来说是同步的，但是对于android来说可以做到异步，都是些申请资源的工作
            self.gen_qemu_sync(out_file)
            


        out_file.write("}\nbreak;\n\n\n")



    def gen_android_sync_create(self, out_file):
        self.gen_android_sync(out_file)

    
    def gen_android_asyn_copy(self, out_file):
        #@todo 异步调用保存数据，减少陷入内核次数

        main_str=f"""
        unsigned char send_buf[16+16*{int(self.get_non_ptr_arg_length()!=0)}];
        size_t send_buf_len=16+16*{int(self.get_non_ptr_arg_length()!=0)};
        uint64_t save_buf_len={int(self.get_non_ptr_arg_length())};
        unsigned char local_save_buf[4096];
        unsigned char *save_buf;
        """
        self.send_items_num=int(self.get_non_ptr_arg_length()!=0)
        for arg in self.in_ptr_args:
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

            if(save_buf_len>4000){{
                save_buf=(unsigned char *)malloc(save_buf_len);
            }}else{{
                save_buf=local_save_buf;
            }}

            unsigned char *ptr=save_buf; 

        """
        
        for arg in self.non_ptr_args:
            main_str += f"""

            *({arg['type']} *)ptr = {arg['name']};
            ptr += {sizeof(arg['type'])};
            """
        for arg in self.in_ptr_args:
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

        *(uint64_t*)ptr=FUNID_{self.name};
        ptr+=sizeof(uint64_t);
        """
        if int(self.get_non_ptr_arg_length())==0:
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
        main_str+=f"""
            send_to_host(context,send_buf,send_buf_len);

            if(save_buf_len>1000){{
                free(save_buf);
            }}

        """

        out_file.write(main_str)


    def gen_android_asyn_no_copy(self, out_file):
        main_str=f"""
        unsigned char send_buf[16+({int(self.get_non_ptr_arg_length()!=0)+len(self.in_ptr_args)})*16];
        size_t send_buf_len=16+({int(self.get_non_ptr_arg_length()!=0)+len(self.in_ptr_args)})*16;
        """

        self.send_items_num=int(self.get_non_ptr_arg_length()!=0)+len(self.in_ptr_args)
        main_str+="unsigned char *ptr=NULL;\n"
 

        if int(self.get_non_ptr_arg_length())<1000 and int(self.get_non_ptr_arg_length())!=0:
            main_str+=f"""

            uint64_t save_buf_len={int(self.get_non_ptr_arg_length())};
            unsigned char *save_buf;
            """
            main_str+=f"""
            unsigned char local_save_buf[{int(self.get_non_ptr_arg_length())}];
            save_buf=local_save_buf;
            ptr=save_buf;
            """
        elif int(self.get_non_ptr_arg_length())>=1000:
            main_str+=f"""

            uint64_t save_buf_len={int(self.get_non_ptr_arg_length())};
            unsigned char *save_buf;
            save_buf=(unsigned char *)malloc(save_buf_len);   
            ptr=save_buf;        
            """
        
        for arg in self.non_ptr_args:
            main_str += f"""

            *({arg['type']} *)ptr = {arg['name']};
            ptr += {sizeof(arg['type'])};
            """

        main_str+=f"""

        ptr=send_buf;

        *(uint64_t*)ptr=FUNID_{self.name};
        ptr+=sizeof(uint64_t);
        """

        # main_str+=f"""
        #     *(uint64_t*)ptr={len(self.in_ptr_args)+len(self.out_ptr_args)};
        #     ptr+=sizeof(uint64_t);
        #     """
        if int(self.get_non_ptr_arg_length())==0:
            main_str+=f"""
            *(uint64_t*)ptr={len(self.in_ptr_args)};
            ptr+=sizeof(uint64_t);
            """
        else:
            main_str+=f"""
            *(uint64_t*)ptr=(uint64_t)(1+{len(self.in_ptr_args)});
            ptr+=sizeof(uint64_t);

            *(uint64_t*)ptr=(uint64_t)save_buf_len;
            ptr+=sizeof(uint64_t);
            *(uint64_t*)ptr=(uint64_t)save_buf;
            ptr+=sizeof(uint64_t);
            """

        for arg in self.in_ptr_args:

            main_str +=f"""

                *(uint64_t*)ptr=(uint64_t){arg['ptr_len']};
                ptr+=sizeof(uint64_t);
                *(uint64_t*)ptr=(uint64_t){arg['name']};
                ptr+=sizeof(uint64_t);
                """
        
        main_str+=f"""

            send_to_host(context,send_buf,send_buf_len);

        """
        # if int(self.get_non_ptr_arg_length())>=1000:
        #     main_str+="free(save_buf);"



        out_file.write(main_str)





    def gen_android_sync(self, out_file):

        

        now_ret_num=0
        if self.type==1:
            now_ret_num=int(self.ret!="")+len(self.out_ptr_args)
        else:
            now_ret_num=int(len(self.out_ptr_args)!=0 or self.ret!="")

        if self.PtrOfPtr:
            ptr_ptr_len = ''
            for arg in self.in_ptr_args:
                if arg['ptr_ptr']:
                    ptr_ptr_len += f"""+{arg['ptr_len'].split("|")[0]}"""
                else:
                    ptr_ptr_len+= "+1"
            main_str = f"""
            unsigned char *send_buf=(unsigned char *)malloc(16+16*({int(self.get_non_ptr_arg_length()!=0)+now_ret_num}{ptr_ptr_len}));
            size_t send_buf_len=16+16*({int(self.get_non_ptr_arg_length()!=0)+now_ret_num}{ptr_ptr_len});
            size_t para_num={int(self.get_non_ptr_arg_length()!=0)+now_ret_num}{ptr_ptr_len};
            """
        else:

            main_str=f"""
            unsigned char send_buf[16+{int(self.get_non_ptr_arg_length()!=0)+len(self.in_ptr_args)+now_ret_num}*16];
            size_t send_buf_len=16+{int(self.get_non_ptr_arg_length()!=0)+len(self.in_ptr_args)+now_ret_num}*16;
            size_t para_num={int(self.get_non_ptr_arg_length()!=0)+len(self.in_ptr_args)+now_ret_num};
            """
        
        self.send_items_num=int(self.get_non_ptr_arg_length()!=0)+len(self.in_ptr_args)+now_ret_num
        



        main_str+="unsigned char *ptr=NULL;\n"

        if int(self.get_non_ptr_arg_length())<1000 and int(self.get_non_ptr_arg_length())!=0:
            main_str+=f"""
            
            uint64_t save_buf_len={int(self.get_non_ptr_arg_length())};
            unsigned char *save_buf;
            """
            main_str+=f"""
            unsigned char local_save_buf[{int(self.get_non_ptr_arg_length())}];
            save_buf=local_save_buf;
            ptr=save_buf;
            """
        elif int(self.get_non_ptr_arg_length())>=1000:
            main_str+=f"""

            uint64_t save_buf_len={int(self.get_non_ptr_arg_length())};
            unsigned char *save_buf;
            save_buf=(unsigned char *)malloc(save_buf_len);   
            ptr=save_buf;        
            """
        
        for arg in self.non_ptr_args:
            main_str += f"""

            *({arg['type']} *)ptr = {arg['name']};
            ptr += {sizeof(arg['type'])};
            """

        main_str+=f"""

        ptr=send_buf;
        *(uint64_t*)ptr=FUNID_{self.name};
        ptr+=sizeof(uint64_t);
        """
        if int(self.get_non_ptr_arg_length())!=0:
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

        for arg in self.in_ptr_args:
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

        if len(self.out_ptr_args)!=0 or self.ret!="":
            if self.type==1:
                for arg in self.out_ptr_args:
                    main_str +=f"""

                    *(uint64_t*)ptr=(uint64_t){arg['ptr_len']};
                    ptr+=sizeof(uint64_t);
                    *(uint64_t*)ptr=(uint64_t){arg['name']};
                    ptr+=sizeof(uint64_t);
                    """
                

                if self.ret != "":
                    main_str +=  f"""

                    {self.ret} ret=0;
                    *(uint64_t*)ptr=(uint64_t)sizeof({self.ret});
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
                for arg in self.out_ptr_args:
                    main_str +=f"""

                    uint64_t {arg['name']}_len=(uint64_t){arg['ptr_len']};
                    out_buf_len+={arg['name']}_len;
                    """
                if self.ret != "":
                    main_str+=f"""
                        out_buf_len+=sizeof({self.ret});
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

            send_to_host(context,send_buf,send_buf_len);


        """
        if int(self.get_non_ptr_arg_length())>=1000:
            main_str+="free(save_buf);\n"

        if self.PtrOfPtr:
            main_str+="free(send_buf);\n"

        if self.type != 1 and (len(self.out_ptr_args)!=0 or self.ret!=""):
            main_str+="out_buf_ptr=out_buf;"
            for arg in self.out_ptr_args:
                main_str +=f"""
                    if({arg['name']}!=NULL){{

                        memcpy({arg['name']},({arg['type']})out_buf_ptr,{arg['name']}_len);
                    }}
                    out_buf_ptr+={arg['name']}_len;
                """
            if self.ret != "":
                main_str+=f"""
                    {self.ret} ret=*({self.ret} *)out_buf_ptr;
                """
            main_str+="""
                if(out_buf_len>MAX_OUT_BUF_LEN){{
                    free(out_buf); 
                }}
            """

        # Return if needed(may require returning ptr)
        if self.ret != '':
            main_str+=f"""
                return ret;\n
            """


        out_file.write(main_str)




    def generate_android_func(self, out_file,context_file):
        # Debug information

        global android_define_line


        out_file.write("\n")
        out_file.write(f"/* readline: \"{self.line}\" */\n")
        out_file.write(f"/* func name: \"{self.name}\" */\n")
        out_file.write(f"/* args: {self.args} */\n")
        out_file.write(f"/* ret: \"{self.ret}\" */\n")
        out_file.write(f"/* type: \"{self.type}\" */\n")
        out_file.write("\n")




        # Write definition string
        def_str = ''
        if self.ret != '':
            def_str = self.ret + ' '
        else:
            def_str = 'void '
        if ( self.type==2 or self.type==5 ) and self.name.find('_origin')==-1:
            def_str += f"d_{self.name}_origin(void *context, "
        else:
            def_str += f"d_{self.name}(void *context, "
        for arg in self.args:
            if arg['ptr_ptr']:
                def_str += f"{arg['type']}const* {arg['name']}, "
            else:
                def_str += f"{arg['type']} {arg['name']}, "
        # if len(self.args) > 0:
        def_str = def_str[:-2]  # Remove last ', '
        all_include.write(def_str+');\n')

        android_define_line+=def_str+");\n"
 
        out_file.write(def_str + ')\n{')

        if ( self.type==2 or self.type==5 ) and self.name.find('_origin')==-1:
            def_str2 = ''
            if self.ret != '':
                def_str2 = self.ret + ' '
            else:
                def_str2 = 'void '
            def_str2 += f"d_{self.name}(void *context, "
            for arg in self.args:
                if arg['ptr_ptr']:
                    def_str2 += f"{arg['type']}const* {arg['name']}, "
                else:
                    def_str2 += f"{arg['type']} {arg['name']}, "
            # if len(self.args) > 0:
            def_str2 = def_str2[:-2]  # Remove last ', '
            all_include.write(def_str2+');\n')

            android_define_line+=def_str2+");\n\n"

            def_str2 += ')\n{\n'

            if self.ret != '':
                def_str2 += 'return '

            def_str2 += f"d_{self.name}_origin(context, "
            for arg in self.args:
                def_str2 += f"{arg['name']}, "
            # if len(self.args) > 0:
            def_str2 = def_str2[:-2]  # Remove last ', '
            def_str2 +=');\n'

            def_str2 +=  '\n}\n\n\n'
            context_file.write(def_str2)


        out_file.write(self.check_error+"\n")


        if self.type ==0 or self.type==1 or self.type==2:
            self.gen_android_sync(out_file)
        elif self.type==3:
            self.gen_android_asyn_copy(out_file)
        elif self.type==4:
            self.gen_android_asyn_no_copy(out_file)
        elif self.type==5:
            self.gen_android_sync_create(out_file)


        

        # End of function
        out_file.write("}\n\n")




define_id_async = 1
define_id_sync = 0

all_func_count = 0


qemu_out = open(f"qemu/all_out.c", "w+")
android_out = open(f"android/all_out.cpp", "w+")

android_context_out = open("android/guest_handle.cpp",'w+')

qemu_define = open(f"qemu/define.h", "w+")
android_define=open(f"android/define_egl.h","w+")

qemu_define_line = ""
# qemu_para_num=open(f"qemu/para_num.h","w+")
android_define_line=""

para_num_line = ""

android_out.write("""
#include "define_egl.h"



#include "../context_common.h"
\n""")


android_context_out.write("""
#include "define_egl.h"

\n""")


qemu_out.write(
    """
#include "express-gpu/egl_define.h"

#include "express-gpu/egl_trans.h"

#include "express-gpu/egl_surface.h"


void egl_decode_invoke(Render_Thread_Context *context,Direct_Express_Call *call)
{
    Render_Thread_Context *render_context=(Render_Thread_Context *)context;


    //Double_Buffer *egl_context = &(render_context->render_double_buffer);

    Call_Para all_para[MAX_PARA_NUM];

    unsigned char ret_local_buf[1024*4];

    unsigned char no_ptr_buf[512];
    
    switch (call->id)
{\n""")
define_id = 100000
for filename in filenames:
    file_type = 0
    if filename == "1-1-1":
        file_type = 0
    elif filename == "1-1-2":
        file_type = 1
    elif filename == "1-2":
        file_type = 2
    elif filename == "2-1-1":
        file_type = 3
    elif filename == "2-1-2":
        file_type = 4
    elif filename == "2-2":
        file_type = 5

    input_file = open(f"in2/{filename}.txt", "r", encoding="utf-8")

    # qemu_out = open(f"qemu/{filename}.h", "w+")

    qemu_out.write(f"\n\n/******* file '{filename}' *******/\n\n\n")
    android_out.write(f"\n\n/******* file '{filename}' *******/\n\n\n")
    android_context_out.write(f"\n\n/******* file '{filename}' *******/\n\n\n")
    

    func_count = 1
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
        func.generate_android_func(android_out,android_context_out)
                    
        # qemu_para_num.write(f"#define PARA_NUM_MIN_{self.name} ({para_num})\n")
        para_num_line += f"#define PARA_NUM_MIN_{func.name} ({func.send_items_num})\n"

        define_id += 1

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

    //if(need_speed){
    call->callback(call, 1);
    //}else{
    //    call->callback(call, 0);
    //}
    return;
}\n""")


qemu_define.write("""


#ifndef EGL_TRANS_H
#define EGL_TRANS_H

#include "direct-express/direct_express_distribute.h"
#include "direct-express/express_device_common.h"
#include "express-gpu/offscreen_render_thread.h"


#define MAX_OUT_BUF_LEN 4096
""")

qemu_define.write(qemu_define_line)
qemu_define.write("\n\n\n\n\n\n\n\n")
qemu_define.write(para_num_line)
qemu_define.write("""
void egl_decode_invoke(Render_Thread_Context *context,Direct_Express_Call *call);

#endif
""")


android_define.write("""

#ifndef DEFINE_EGL_H
#define DEFINE_EGL_H

#include <EGL/egl.h>


# define EXPRESS_GPU_FUN_ID ((unsigned long long)1)


#define MAX_OUT_BUF_LEN 4096\n
""")

android_define.write(android_define_line)
android_define.write("\n\n\n\n\n\n\n\n")
android_define.write("""
#endif
""")


all_include.write("\n#endif")

all_include.close()

qemu_out.close()
qemu_define.close()
# qemu_para_num.close()
