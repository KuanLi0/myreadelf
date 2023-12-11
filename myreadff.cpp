#include<iostream>
#include<fstream>
#include<iomanip>
#include<elf.h>
#include<string.h>
#include<string>
using namespace std;

typedef struct
{
  int symboloff;//记录符号表的偏移量
  int stroff;//记录字符串表的偏移量
  int shstroff;//记录段字符串表的偏移量。
  int symbolsize;//符号表的大小
  int strtabsize;//字符串表的大小
  int shsstrxize;//记录段字符串表的大小。
} SH_STR_OFF;//用这个结构体主要目的是记录偏移量与符号表、字符串表的大小,方便后续寻址。

SH_STR_OFF elf_shstroff;//为读取字符串表和符号表h做准备
Elf64_Shdr elfsh;  //段表结构体。
Elf64_Sym  elfsym;//符号表结构体。
Elf64_Ehdr elf_Header;//用来接受elf header
//读取header
void header_in(Elf64_Ehdr & elf,char *arg,char*path){

    ifstream inFile(path, ios::in | ios::binary);
    if(! inFile){
        cout   <<"打开文件失败！"<<endl;

    }

    inFile.read((char *)&elf, sizeof(elf));
    if(strcmp(arg, "h")==0||strcmp(arg, "a")==0){
        cout<<"elf文件类型：\t\t\t\t"<<(elf.e_type==ET_REL?"可重定位":
        (elf.e_type==ET_EXEC?"可执行文件":"共享目标文件"))<<endl;
        cout<<"elf文件段字符串表在段中的下标：\t\t"<<elf.e_shstrndx<<endl;
        cout<<"elf文件入口地址：\t\t\t"<<elf.e_entry<<endl;
        cout<<"elf文件段表在文件中的偏移：\t\t"<<elf.e_shoff<<endl;
        cout<<"elf文件段表描述符的数量：\t\t"<<elf.e_shnum<<endl;//结构体数组有多少个数组。
        cout<<"elf文件属性：\t\t\t\t"<<elf.e_flags<<endl;
        cout<<"elf文件机械类型：\t\t\t\t"<<(elf.e_flags==EM_M32?"AT&T":(elf.e_flags==EM_SPARC?"SPARC"
        :(elf.e_flags==EM_386?"intel x86":(elf.e_flags==EM_68K?"Motorola 680000":(elf.e_flags==EM_88K?
        "Motorola 88000":"intel 80860")))))<<endl;
    }
    inFile.close();
}



//单个段字符串

string read_sshstr(SH_STR_OFF elf_shstroff,int arr,char*path){
  
    ifstream inFile(path, ios::in | ios::binary);
    if(! inFile){
        cout   <<"打开文件失败！"<<endl;

    }
       
        inFile.seekg(elf_shstroff.shstroff,ios::beg);//定位到刚开始
        inFile.seekg(arr,ios::cur);
        char str[50];
        memset(str,'\0',sizeof(str));//清空数组
        inFile.get(str,50,'\0');
        
    inFile.close();
    return str;

}










void readSH(Elf64_Shdr &elfsh,SH_STR_OFF &elf_shoff,int sh_off,int sh_num,
int str_ind,int aarr[],char *arg,char*path){


    ifstream inFile(path, ios::in | ios::binary);
    if(! inFile){
        cout   <<"打开文件失败！"<<endl;

    }
    inFile.seekg(sh_off,ios::beg);
 
    int arr_size[sh_num];
    int arr_add[sh_num];
    int arr_type[sh_num];
    int arr_off[sh_num];
      for (int i = 0; i < sh_num; i++)//用for循环依次拿取
    {   
         inFile.read((char *)&elfsh, sizeof(elfsh));
         arr_size[i]=elfsh.sh_size;
         arr_add[i]=elfsh.sh_addr;
         arr_type[i]=elfsh.sh_type;
         arr_off[i]=elfsh.sh_offset;
         
         aarr[i]=elfsh.sh_name;//用来存储段的名字在段字符串中的偏移
         if(elfsh.sh_type==SHT_SYMTAB){

            elf_shoff.symboloff=elfsh.sh_offset;//记录符号表的偏移量
            elf_shoff.symbolsize=elfsh.sh_size;//记录符号表的大小
              //cout<< elf_shoff.symboloff<<endl;
         }
         if(elfsh.sh_type==SHT_STRTAB&&i!=str_ind){

            elf_shoff.stroff=elfsh.sh_offset;//记录字符串表的偏移量
            elf_shoff.strtabsize=elfsh.sh_size;//记录字符串表的大小
              //cout<<elf_shoff.stroff<<endl;
         }
         if(i==str_ind){

            elf_shoff.shstroff=elfsh.sh_offset;//记录段字符串表的偏移量。_
            elf_shoff.shsstrxize=elfsh.sh_size;
            //elf_shoff.=elfsh.sh_size;
            //cout<<elf_shoff.shstroff<<endl;
         }
        if(strcmp(arg, "s")==0||strcmp(arg, "a")==0){
          
            if(i==(sh_num-1)){
                cout<<"\t索引\t"<<"名称"<<"\t\t"<<"大小\t"<<"偏移\t"<<"虚拟地址\t"<<"类型"<<endl;
                for (int k = 0; k < sh_num; k++)
                {
                    cout<<"\t"<<k<<"\t"<<read_sshstr(elf_shstroff,aarr[k],path)<<"\t\t"
                    <<arr_size[k]<<"\t"<<arr_off[k]<<"\t"<<arr_add[k]<<"\t\t"<<(arr_type[k]==SHT_NULL?"无效段":(arr_type[k]==SHT_PROGBITS
                     ?"程序段、代码段、数据段":(arr_type[k]==SHT_STRTAB?"字符串表"
                    :(arr_type[k]==SHT_SYMTAB?"符号表":"其他类型"))))<<endl;

                }

            }
        }
    }
    inFile.close();
   
    cout<<"*******************打印段表完毕*****************"<<endl;

}



//读取段字符串表
void read_shstr(SH_STR_OFF elf_shstroff,int arr[],int sh_num,char *arg,char*path){
  
    cout<<"*******************打印段字符串表*****************"<<endl;
    cout<<"*******************打印段字符串表*****************"<<endl;
    ifstream inFile(path, ios::in | ios::binary);


    if(! inFile){
        cout   <<"打开文件失败！"<<endl;

    }
    //inFile.seekg(elf_shstroff.shstroff+1,ios::beg);//定位到刚开始
    //cout<<inFile.tellg()<<endl;
    inFile.seekg(elf_shstroff.shstroff,ios::beg);//定位到刚开始
       while (1) {
         
            inFile.seekg(1,ios::cur);//越过‘\0’
            char str[50];
            memset(str,'\0',sizeof(str));//清空数组
            inFile.get(str,50,'\0');
            for(int i =0;i<sizeof(str);i++){
            cout<<str[i];//输出的时候少个点。
            }
            cout<<endl;
            if(inFile.tellg()>=elf_shstroff.shstroff+elf_shstroff.shsstrxize-1){
                break;
            }
      }
    cout<<"***************************************************"<<endl;
       for (int i = 1; i < sh_num; i++)//一定要让i=1,因为arr[0]=0
     {  
        
        inFile.seekg(elf_shstroff.shstroff,ios::beg);//定位到刚开始
        inFile.seekg(arr[i],ios::cur);
        char str[50];
        memset(str,'\0',sizeof(str));//清空数组
        inFile.get(str,50,'\0');
        if(strcmp(arg, "a")==0||strcmp(arg, "shs")==0){

            for(int i =0;i<sizeof(str);i++){
                cout<<str[i];//输出的时候少个点。
            }
            cout<<endl;
        }
       
    }
     
    inFile.close();
    cout<<"*******************打印段字符串表完毕*****************"<<endl;

}



//读取字符串表
void read_str(SH_STR_OFF elf_shstroff,char*path){
    cout<<"*******************打印字符串表*****************"<<endl;
    cout<<"*******************打印字符串表*****************"<<endl;

    ifstream inFile(path, ios::in | ios::binary);
    if(! inFile){
        cout   <<"打开文件失败！"<<endl;

    }
    inFile.seekg(elf_shstroff.stroff,ios::beg);//定位到刚开始
       while (1) {
         
            inFile.seekg(1,ios::cur);//定位到刚开始
            char str[50];
            memset(str,'\0',sizeof(str));//清空数组
            inFile.get(str,50,'\0');
            for(int i =0;i<sizeof(str);i++){
            cout<<str[i];//输出的时候少个点。
            }
            cout<<endl;
            if(inFile.tellg()>=elf_shstroff.stroff+elf_shstroff.strtabsize-1){
                break;
            }
      }
    cout<<"*******************打印字符串表完毕*****************"<<endl;
       
}

//重载函数
string read_str(SH_STR_OFF elf_shstroff,char*path,int shoff){

    ifstream inFile(path, ios::in | ios::binary);
    if(! inFile){
        cout   <<"打开文件失败！"<<endl;

    }
    inFile.seekg(elf_shstroff.stroff,ios::beg);//定位到刚开始
     
         
            inFile.seekg(shoff,ios::cur);//定位到目标符号
            char str[50];
            memset(str,'\0',sizeof(str));//清空数组
            inFile.get(str,50,'\0');
    
    
    return str;  
}



//读取符号表
void read_symbol(SH_STR_OFF elf_shstroff,char*path){
    cout<<"*******************打印符号表*****************"<<endl;
    cout<<"*******************打印符号表*****************"<<endl;
    ifstream inFile(path, ios::in | ios::binary);
    if(! inFile){
        cout   <<"打开文件失败！"<<endl;

    }
    inFile.seekg(elf_shstroff.symboloff,ios::beg);//定位到刚开始
    
    int index=0;//记录符号的序号
    cout<<"\t索引\t"<<"名称"<<"\t\t"<<"大小\t"<<"所在段\t"<<"符号值\t"<<endl;
    while (1) {
        
            inFile.read((char *)&elfsym, sizeof(elfsym));
            cout<<"\t"<<index<<"\t"<<read_str(elf_shstroff,path,elfsym.st_name)<<"\t\t"<<elfsym.st_size<<"\t"<<elfsym.st_shndx
            <<"\t"<<elfsym.st_value<<endl;//输出格式有点问题

            index++;
            if(inFile.tellg()>=elf_shstroff.symboloff+elf_shstroff.symbolsize-1){
                break;
            }
      }
    inFile.close();
    cout<<"*******************打印符号表完毕*****************"<<endl;
}
int main(int argc,char**argv){  //agrv[1]代表参数 h打印header a打印全部 s打印段表 shs段字符串 str字符串表 
//sym 打印符号
    
    
    if(strcmp(argv[1], "h")==0){
        
        header_in(elf_Header,argv[1],argv[2]);//读取并输出头文件
    }
    else if(strcmp(argv[1], "s")==0){
        //打印段表
        header_in(elf_Header,argv[1],argv[2]);
        //为段表做读取准备

        int arr[elf_Header.e_shnum];//用来存储段的名字在段字符串中的偏移
        readSH(elfsh,elf_shstroff,elf_Header.e_shoff,elf_Header.e_shnum,elf_Header.e_shstrndx,arr,argv[1],argv[2]);
    }
    else if(strcmp(argv[1], "shs")==0){//打印段字符串
        header_in(elf_Header,argv[1],argv[2]);
        //为段表做读取准备

       
        int arr[elf_Header.e_shnum];//用来存储段的名字在段字符串中的偏移
        readSH(elfsh,elf_shstroff,elf_Header.e_shoff,elf_Header.e_shnum,elf_Header.e_shstrndx,arr,argv[1],argv[2]);

        read_shstr(elf_shstroff,arr,elf_Header.e_shnum,argv[1],argv[2]);
    }
    else if(strcmp(argv[1], "str")==0){//{打印字符串
        header_in(elf_Header,argv[1],argv[2]);
        //为段表做读取准备

        
        int arr[elf_Header.e_shnum];//用来存储段的名字在段字符串中的偏移
        readSH(elfsh,elf_shstroff,elf_Header.e_shoff,elf_Header.e_shnum,elf_Header.e_shstrndx,arr,argv[1],argv[2]);
        read_str(elf_shstroff,argv[2]);    //读段字符串表
    }
    else if(strcmp(argv[1], "sym")==0){//打印符号表
        header_in(elf_Header,argv[1],argv[2]);
     
        
        int arr[elf_Header.e_shnum];//用来存储段的名字在段字符串中的偏移
      
        readSH(elfsh,elf_shstroff,elf_Header.e_shoff,elf_Header.e_shnum,elf_Header.e_shstrndx,arr,argv[1],argv[2]);
        read_symbol(elf_shstroff,argv[2]);    //读取符号表
    }
     else if(strcmp(argv[1], "a")==0){
        header_in(elf_Header,argv[1],argv[2]);
        //输出全部


        int arr[elf_Header.e_shnum];//用来存储段的名字在段字符串中的偏移
        readSH(elfsh,elf_shstroff,elf_Header.e_shoff,elf_Header.e_shnum,elf_Header.e_shstrndx,arr,argv[1],argv[2]);
        read_shstr(elf_shstroff,arr,elf_Header.e_shnum,argv[1],argv[2]);
        read_str(elf_shstroff,argv[2]);    //读取段字符串表
        read_symbol(elf_shstroff,argv[2]);   //读取符号表
    }
    
    system("pause");
    return 0;
}
