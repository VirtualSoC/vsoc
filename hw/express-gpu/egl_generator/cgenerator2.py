#!/bin/python3
# -*- coding: UTF-8 -*-

import sys

# gl_pixel_data_size(width,height,format,type)
filenames = [
    "1-1",  # 同步非快速
    "1-2",  # 同步快速
    "2-1",  # 异步需保存
    "2-2",  # 异步不需保存
]

# 64-bit target?
target64bit = True

ptrbits = 64 if target64bit else 32
sizeof_dic = {
    'EGLBoolean': 32,     'EGLenum': 32,           'EGLTime': 64,
    'EGLInt':32,
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
        temp = self.line.split()

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
            self.send_items_num = len(self.in_ptr_args)
            self.send_items_num += len(self.out_ptr_args)
            if len(self.non_ptr_args) != 0:
                self.send_items_num += 1
            if self.type == 2:
                self.send_items_num = 1
        # if self.ret != "":
        #     self.send_items_num += 1

    def get_non_ptr_arg_length(self):
        # Get size of non ptr arguments
        non_ptr_arg_length = 0
        for arg in self.non_ptr_args:
            non_ptr_arg_length += sizeof(arg['type'])
        return non_ptr_arg_length






    def gen_qemu_asyn_save(self, out_file):
        non_ptr_args_length = f"{int(self.get_non_ptr_arg_length())}"
        if non_ptr_args_length != "0" or len(self.args)!=0:

            main_str = f"""
                size_t temp_len=0;
                unsigned char *temp=NULL;

                int need_delete=0;

                temp_len=all_para[0].data_len;
                if(temp_len< {non_ptr_args_length} * {sizeof('GLbyte')}){{
                    break;
                }}
                
                int null_flag=0;
                temp=get_direct_ptr(all_para[0].data,&null_flag);
                if(temp==NULL){{
                    if(temp_len!=0 &&null_flag==0){{
                        temp=g_malloc(all_para[0].data_len);
                        guest_write(all_para[0].data,temp,0,all_para[0].data_len);
                        need_delete=1;

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

                main_str += f"""
                    {arg['type']} {arg['name']}=({arg['type']})(temp+temp_loc);
                    temp_loc+={arg['ptr_len']};
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
        out_file.write(main_str+"\n\n"+call_str)
        if non_ptr_args_length != "0" or len(self.args)!=0:
            out_file.write("""\n\nif(need_delete){g_free(temp);}\n""")






    def gen_qemu_asyn_no_save(self, out_file):
        # non_ptr_args_length = f"{int(self.get_non_ptr_arg_length())}"
        # if non_ptr_args_length != "0":

        #     main_str = f"""
        #         size_t temp_len=0;
        #         unsigned char *temp=NULL;

        #         int need_delete=0;

        #         temp_len=all_para[0].data_len;
        #         if(temp_len< {non_ptr_args_length} * {sizeof('GLbyte')}){{
        #             break;
        #         }}

        #         int null_flag=0;
        #         temp=get_direct_ptr(all_para[0].data,&null_flag);
        #         if(temp==NULL){{
        #             if(temp_len!=0 && null_flag==0){{
        #                 temp=g_malloc(all_para[0].data_len);
        #                 guest_write(all_para[0].data,temp,0,all_para[0].data_len);

        #                 need_delete=1;
        #             }}else{{
        #                 break;
        #             }}
        #         }}
                

        #         unsigned int temp_loc=0;
        #     """
        # else:
        #     main_str=""

        # for arg in self.args:
        #     # Read non-ptr variable
        #     if arg['ptr'] == 'NA':
        #         main_str += f"""

        #             {arg['name']} = *({arg['type']} *)(temp+temp_loc);
        #             temp_loc+={sizeof(arg['type'])};
        #         """

        main_str=""

        loc = 0
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

        if self.name.find("_") == -1 or self.name.find("_v") != -1:
            call_str = f"{self.name}("
        else:
            call_str = f"d_{self.name}(render_context,"
        for arg in self.args:
            if arg['ptr'] != 'NA':
                call_str += f"{arg['name']}, "
        if len(self.args) > 0:
            call_str = call_str[:-2]  # Remove last ', '
        call_str += ');\n'
        out_file.write(main_str+"\n\n"+call_str)
        # if non_ptr_args_length != "0":
        #     out_file.write("""\n\nif(need_delete){g_free(temp);}\n""")







    def gen_qemu_sync(self, out_file):
        non_ptr_args_length = f"{int(self.get_non_ptr_arg_length())}"
        if non_ptr_args_length != "0":
            main_str = f"""
                size_t temp_len=0;
                unsigned char *temp=NULL;

                int need_delete=0;
                temp_len=all_para[0].data_len;
                if(temp_len< {non_ptr_args_length} * {sizeof('GLbyte')}){{
                    break;
                }}

                int null_flag=0;
                temp=get_direct_ptr(all_para[0].data,&null_flag);
                if(temp==NULL){{
                    if(temp_len!=0 && null_flag==0){{
                        temp=g_malloc(all_para[0].data_len);
                        guest_write(all_para[0].data,temp,0,all_para[0].data_len);

                        need_delete=1;
                    }}else{{
                        break;
                    }}
                }}

            """
            main_str += "unsigned int temp_loc=0;"
        else:
            main_str = f""""""

        for arg in self.args:
            # Read non-ptr variable
            if arg['ptr'] == 'NA':
                main_str += f"""

                    {arg['name']} = *({arg['type']} *)(temp+temp_loc);
                    temp_loc+={sizeof(arg['type'])};
                """

        loc = 1
        in_ptr_get = 0
        ptr_num = ""

        ptr_loc=""
        for arg in self.args:
            # Read ptr variable
            if arg['ptr'] == 'in':

                if self.name.find("_") != -1 and self.name.find("_v") == -1 and self.name.find("_origin") == -1:
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
                

        if len(self.out_ptr_args) != 0:
            if self.name.find("_") == -1 or self.name.find("_v") != -1 or self.name.find("_origin") != -1:
                main_str += "\nint out_buf_len=0;\n\n"
                temp_loc = loc
                for arg in self.args:
                    # Read ptr variable
                    if arg['ptr'] == 'out':
                        main_str += f"out_buf_len+=all_para[{loc}{ptr_loc}].data_len;"
                        loc += 1
                loc = temp_loc

                main_str += f"""

                    unsigned char *ret_buf=NULL;

                    if(out_buf_len>MAX_OUT_BUF_LEN){{
                        ret_buf=g_malloc(out_buf_len);
                    }}else{{
                        ret_buf=ret_local_buf;
                    }}
                    out_buf_len=0;
                
                """

                temp_loc = loc
                for arg in self.args:
                    # Read ptr variable
                    if arg['ptr'] == 'out':
                        main_str += f"""

                            {arg['type']} {arg['name']}=({arg['type']})(ret_buf+out_buf_len);
                            if(all_para[{loc}{ptr_loc}].data_len==0){{
                                //is null ptr
                                {arg['name']}=NULL;
                            }}
                            out_buf_len+=all_para[{loc}{ptr_loc}].data_len;
                        """
                        loc += 1
                loc = temp_loc
            else:
                temp_loc = loc
                for arg in self.args:
                    # Read ptr variable
                    if arg['ptr'] == 'out':
                        main_str += f"""

                            {arg['type']} {arg['name']}=all_para[{loc}{ptr_loc}].data;
                        """
                        loc += 1
                loc = temp_loc

        if self.name.find("_") == -1 or self.name.find("_v") != -1:
            call_str = f"{self.name}("
        else:
            call_str = f"d_{self.name}(render_context,"
        for arg in self.args:
            call_str += f"{arg['name']}, "
        if len(self.args) > 0:
            call_str = call_str[:-2]  # Remove last ', '
        call_str += ');\n'
        if self.ret != '':
            call_str = f"""

                {self.ret} ret = {call_str}
                set_call_return_val(call,(unsigned char *)&ret, {sizeof(self.ret)});
                """

        out_file.write(main_str+"\n\n"+call_str)
        main_str = ""

        # 写回数据
        if self.name.find("_") == -1 or self.name.find("_v") != -1:
            temp_loc = loc
            for arg in self.args:
                # Read ptr variable
                if arg['ptr'] == 'out':

                    main_str += f"""

                        guest_read(all_para[{loc}{ptr_loc}].data,{arg['name']},0,all_para[{loc}{ptr_loc}].data_len);
                    """
                    loc += 1
            loc = temp_loc

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

        if non_ptr_args_length != "0":
            out_file.write("""\n\nif(need_delete){g_free(temp);}\n""")


        if len(self.out_ptr_args) != 0 and self.name.find("_") == -1 or self.name.find("_v") != -1:
            out_file.write("\n\nif(out_buf_len>MAX_OUT_BUF_LEN){g_free(ret_buf);}")

    def generate_qemu_case(self, out_file, define_id):
        global define_line
        global para_num_line

        if self.ret != '':
            flag_str="0x8"
        else:
            flag_str="0"

        if self.type == 0:
            flag_str+="+0x4"
        elif self.type == 2:
            flag_str+="+0x2+0x1"
        elif self.type == 3:
            flag_str+="+0x1"
            if len(self.non_ptr_args)!=0:
                self.send_items_num-=1
        
        define_line += f"#define FUNID_{self.name} ((EXPRESS_GPU_FUN_ID<<32u)+(((unsigned long long){flag_str})<<24u)+{define_id})\n"

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
        if self.type == 0 or self.type == 1:
            # 同步
            self.gen_qemu_sync(out_file)
        elif self.type == 2:
            # 异步需保存，就一个参数
            self.gen_qemu_asyn_save(out_file)
        elif self.type == 3:
            # 异步不需要保存，可能有多个参数
            self.gen_qemu_asyn_no_save(out_file)

        out_file.write("}\nbreak;\n\n\n")





    
    def gen_android_asyn_save(self, out_file):
        #@todo 异步调用保存数据，减少陷入内核次数

        main_str=f"""
        unsigned char send_buf[32];
        size_t send_buf_len=32;
        uint64_t save_buf_len={int(self.get_non_ptr_arg_length())};
        unsigned char local_save_buf[1024];
        unsigned char *save_buf;
        """
        for arg in self.in_ptr_args:
            main_str += f"""

            size_t {arg['name']}_len={arg['ptr_len']};
            save_buf_len+={arg['name']}_len;
            """

        main_str+=f"""

            if(save_buf_len>1000){{
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
            main_str += f"""

            memcpy(ptr,(unsigned char *){arg['name']},{arg['name']}_len);
            ptr+={arg['name']}_len;
            """

        main_str+=f"""

        ptr=send_buf;

        *(uint64_t*)ptr=FUNID_{self.name};
        ptr+=sizeof(uint64_t);
        if(save_buf_len==0){{
            *(uint64_t*)ptr=0;
            ptr+=sizeof(uint64_t);

        }}else{{
            *(uint64_t*)ptr=1;
            ptr+=sizeof(uint64_t);

            *(uint64_t*)ptr=(uint64_t)save_buf_len;
            ptr+=sizeof(uint64_t);
            *(uint64_t*)ptr=(uint64_t)save_buf;
            ptr+=sizeof(uint64_t);
        }}

        send_to_host(context,send_buf,send_buf_len);

        if(save_buf_len>1000){{
            free(save_buf);
        }}

        """

        out_file.write(main_str)


    def gen_android_asyn_no_save(self, out_file):
        main_str=f"""
        unsigned char send_buf[32+{len(self.in_ptr_args)+len(self.out_ptr_args)}*16];
        size_t send_buf_len=32+{len(self.in_ptr_args)+len(self.out_ptr_args)}*16;
        """
        # main_str+="unsigned char *ptr=NULL;\n"
        # if int(self.get_non_ptr_arg_length())==0:
        #     main_str+=f"""

        #     send_buf_len-=16;
        #     """


        # if int(self.get_non_ptr_arg_length())<1000 and int(self.get_non_ptr_arg_length())!=0:
        #     main_str+=f"""

        #     uint64_t save_buf_len={int(self.get_non_ptr_arg_length())};
        #     unsigned char *save_buf;
        #     """
        #     main_str+=f"""
        #     unsigned char local_save_buf[{int(self.get_non_ptr_arg_length())}];
        #     save_buf=local_save_buf;
        #     ptr=save_buf;
        #     """
        # elif int(self.get_non_ptr_arg_length())>=1000:
        #     main_str+=f"""

        #     uint64_t save_buf_len={int(self.get_non_ptr_arg_length())};
        #     unsigned char *save_buf;
        #     save_buf=(unsigned char *)malloc(save_buf_len);   
        #     ptr=save_buf;        
        #     """
        
        # for arg in self.non_ptr_args:
        #     main_str += f"""

        #     *({arg['type']} *)ptr = {arg['name']};
        #     ptr += {sizeof(arg['type'])};
        #     """

        main_str+=f"""

        unsigned char *ptr=send_buf;

        *(uint64_t*)ptr=FUNID_{self.name};
        ptr+=sizeof(uint64_t);
        """

        main_str+=f"""
            *(uint64_t*)ptr={len(self.in_ptr_args)+len(self.out_ptr_args)};
            ptr+=sizeof(uint64_t);
            """
        # if int(self.get_non_ptr_arg_length())==0:
        #     main_str+=f"""
        #     *(uint64_t*)ptr={len(self.in_ptr_args)};
        #     ptr+=sizeof(uint64_t);
        #     """
        # else:
        #     main_str+=f"""
        #     *(uint64_t*)ptr=(uint64_t)(1+{len(self.in_ptr_args)});
        #     ptr+=sizeof(uint64_t);

        #     *(uint64_t*)ptr=(uint64_t)save_buf_len;
        #     ptr+=sizeof(uint64_t);
        #     *(uint64_t*)ptr=(uint64_t)save_buf;
        #     ptr+=sizeof(uint64_t);
        #     """

        for arg in self.in_ptr_args:

            main_str +=f"""

                *(uint64_t*)ptr=(uint64_t){arg['ptr_len']};
                ptr+=sizeof(uint64_t);
                *(uint64_t*)ptr=(uint64_t){arg['name']};
                ptr+=sizeof(uint64_t);
                """
        for arg in self.out_ptr_args:

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

        if self.PtrOfPtr:
            ptr_ptr_len = ''
            for arg in self.in_ptr_args:
                if arg['ptr_ptr']:
                    ptr_ptr_len += f"""+{arg['ptr_len'].split("|")[0]}"""
                else:
                    ptr_ptr_len+= "+1"
            main_str = f"""
            unsigned char *send_buf=(unsigned char *)malloc(32+16+16*({len(self.out_ptr_args)}{ptr_ptr_len}));
            size_t send_buf_len=32+16*({len(self.out_ptr_args)}{ptr_ptr_len});
            size_t para_num=1+{len(self.out_ptr_args)}{ptr_ptr_len};
            """
        else:
            main_str=f"""
            unsigned char send_buf[32+16+{len(self.in_ptr_args)+len(self.out_ptr_args)}*16];
            size_t send_buf_len=32+{len(self.in_ptr_args)+len(self.out_ptr_args)}*16;
            size_t para_num=1+{len(self.in_ptr_args)+len(self.out_ptr_args)};
            """
        if int(self.get_non_ptr_arg_length())==0:
            main_str+=f"""

            send_buf_len-=16;
            para_num-=1;

            """

        if self.ret!="":
            main_str+="send_buf_len+=16;\n"
        
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
            uint64_t ret_long=0;
            *(uint64_t*)ptr=(uint64_t)sizeof(uint64_t);
            ptr+=sizeof(uint64_t);
            *(uint64_t*)ptr=(uint64_t)&ret_long;
            ptr+=sizeof(uint64_t);

        """


        main_str+=f"""

            send_to_host(context,send_buf,send_buf_len);


        """
        if int(self.get_non_ptr_arg_length())>=1000:
            main_str+="free(save_buf);\n"

        if self.PtrOfPtr:
            main_str+="free(send_buf);\n"

        # Return if needed(may require returning ptr)
        if self.ret != '':
            main_str+=f"""
            //ret =({self.ret})ret_long;
            memcpy(&ret,&ret_long,sizeof({self.ret}));
            return ret;\n"""


        out_file.write(main_str)




    def generate_android_func(self, out_file):
        # Debug information
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


        if self.type ==0 or self.type==1:
            self.gen_android_sync(out_file)
        elif self.type==2:
            self.gen_android_asyn_save(out_file)
        elif self.type==3:
            self.gen_android_asyn_no_save(out_file)


        

        # End of function
        out_file.write("}\n\n")




define_id_async = 1
define_id_sync = 0

all_func_count = 0


qemu_out = open(f"qemu/all_out.c", "w+")
android_out = open(f"android/all_out.cpp", "w+")

qemu_define = open(f"qemu/define.h", "w+")
define_line = ""
# qemu_para_num=open(f"qemu/para_num.h","w+")
para_num_line = ""

android_out.write("""

#include "egl_define.h"
#include "define.h"

\n""")

qemu_out.write(
    """

#include "express-gpu/egl_trans.h"

#include "express-gpu/egl_surface.h"


void egl_decode_invoke(Render_Thread_Context *context,Direct_Express_Call *call)
{
    Render_Thread_Context *render_context=(Render_Thread_Context *)context;
    //Double_Buffer *egl_context = &(render_context->render_double_buffer);
    //uint64_t fun_id=GET_FUN_ID(call->id);
    //uint64_t is_async=FUN_IS_ASYNC(call->id);
    uint64_t need_speed=FUN_NEED_SPEED(call->id);
    Call_Para all_para[MAX_PARA_NUM];

    unsigned char ret_local_buf[1024*4];
    
    switch (call->id)
{\n""")
define_id = 100000
for filename in filenames:
    file_type = 0
    if filename == "1-1":
        file_type = 0
    elif filename == "1-2":
        file_type = 1
    elif filename == "2-1":
        file_type = 2
    elif filename == "2-2":
        file_type = 3

    input_file = open(f"in/{filename}.txt", "r", encoding="utf-8")
    # qemu_out = open(f"qemu/{filename}.h", "w+")

    qemu_out.write(f"\n\n/******* file '{filename}' *******/\n\n\n")
    android_out.write(f"\n\n/******* file '{filename}' *******/\n\n\n")

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
        func.generate_android_func(android_out)
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

    if(need_speed){
        call->callback(call, 1);
    }else{
        call->callback(call, 0);
    }
    return;
}\n""")


qemu_define.write("""

//#ifndef DEFINE_H
//#define DEFINE_H

#ifndef EGL_TRANS_H
#define EGL_TRANS_H

#include "direct-express/direct_express_distribute.h"
#include "direct-express/express_device_common.h"
#include "express-gpu/offscreen_render_thread.h"


//android
#include <EGL/egl.h>
#include <EGL/eglplatform.h>



//# define EXPRESS_GPU_FUN_ID ((unsigned long long)1)


#define MAX_OUT_BUF_LEN 4000\n
""")

qemu_define.write(define_line)
qemu_define.write("\n\n\n\n\n\n\n\n")
qemu_define.write(para_num_line)
qemu_define.write("""
void egl_decode_invoke(Render_Thread_Context *context,Direct_Express_Call *call);

#endif
""")


all_include.write("\n#endif")

all_include.close()

qemu_out.close()
qemu_define.close()
# qemu_para_num.close()
