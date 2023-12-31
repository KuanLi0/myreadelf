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

SH_STR_OFF elf_shstroff;//为读取字符串表和符号表做准备
Elf64_Shdr elfsh;  //段表结构体。
Elf64_Sym  elfsym;//符号表结构体。
Elf64_Ehdr elf_Header;//用来接受elf header
//读取header
void header_in(Elf64_Ehdr & elf,char *arg,char*path){
    //打开文件
    ifstream inFile(path, ios::in | ios::binary);
    if(! inFile){
        cout   <<"打开文件失败！"<<endl;

    }

    //将文件中的内容输出到自己创建的elfeoe结构体中
    inFile.read((char *)&elf, sizeof(elf));

    //根据用户的选择来判断是否输出
    if(strcmp(arg, "h")==0||strcmp(arg, "a")==0){
        cout<<"elf文件类型：\t\t\t\t"<<(elf.e_type==ET_REL?"可重定位":
        (elf.e_type==ET_EXEC?"可执行文件":"共享目标文件"))<<endl;
        cout<<"elf文件段字符串表在段中的下标：\t\t"<<elf.e_shstrndx<<endl;
        cout<<"elf文件入口地址：\t\t\t"<<elf.e_entry<<endl;
        cout<<"elf文件段表在文件中的偏移：\t\t"<<elf.e_shoff<<endl;
        cout<<"elf文件段表描述符的数量：\t\t"<<elf.e_shnum<<endl;//结构体数组有多少个数组。
        cout<<"elf文件属性：\t\t\t\t"<<elf.e_flags<<endl;
        cout<<"elf文件机械类型：\t\t\t"<<(elf.e_flags==EM_M32?"AT&T":(elf.e_flags==EM_SPARC?"SPARC"
        :(elf.e_flags==EM_386?"intel x86":(elf.e_flags==EM_68K?"Motorola 680000":(elf.e_flags==EM_88K?
        "Motorola 88000":"intel 80860")))))<<endl;
    }

    //关闭文件
    inFile.close();
}



//单个段字符串读取
string read_sshstr(SH_STR_OFF elf_shstroff,int arr,char*path){
    //打开文件
    ifstream inFile1(path, ios::in | ios::binary);
    if(! inFile1){
        cout   <<"打开文件失败！"<<endl;
    } 
        //定位文件指针
        inFile1.seekg(elf_shstroff.shstroff,ios::beg);//从开始定位
        inFile1.seekg(arr,ios::cur);//从当前位置再次偏移

        //创建字符串接受段名
        char str[50];
        memset(str,'\0',sizeof(str));//初始化数组
        inFile1.get(str,50,'\0');//获取字符串
        
    inFile1.close();
    return str;//返回

}


//读取段表
void readSH(Elf64_Shdr &elfsh,SH_STR_OFF &elf_shoff,int sh_off,int sh_num,
int str_ind,int aarr[],char *arg,char*path){


    ifstream inFile(path, ios::in | ios::binary);
    if(! inFile){
        cout   <<"打开文件失败！"<<endl;

    }
    //定位到段表的起始处
    inFile.seekg(sh_off,ios::beg);

    //定义一系列数组，用来存储每个段的关键数据
    int arr_size[sh_num];
    int arr_add[sh_num];
    int arr_type[sh_num];
    int arr_off[sh_num];

    for (int i = 0; i < sh_num; i++)//用for循环依次拿取
    {   
         inFile.read((char *)&elfsh, sizeof(elfsh));//读取，将值传递给elfsh
         arr_size[i]=elfsh.sh_size;
         arr_add[i]=elfsh.sh_addr;
         arr_type[i]=elfsh.sh_type;
         arr_off[i]=elfsh.sh_offset;
         
         aarr[i]=elfsh.sh_name;//用来存储段的名字在段字符串中的偏移
         if(elfsh.sh_type==SHT_SYMTAB){

            elf_shoff.symboloff=elfsh.sh_offset;//记录符号表的偏移量
            elf_shoff.symbolsize=elfsh.sh_size;//记录符号表的大小
            
         }
         if(elfsh.sh_type==SHT_STRTAB&&i!=str_ind){

            elf_shoff.stroff=elfsh.sh_offset;//记录字符串表的偏移量
            elf_shoff.strtabsize=elfsh.sh_size;//记录字符串表的大小
            
         }
         if(i==str_ind){

            elf_shoff.shstroff=elfsh.sh_offset;//记录段字符串表的偏移量。
            elf_shoff.shsstrxize=elfsh.sh_size;
            
         }
        if(strcmp(arg, "s")==0||strcmp(arg, "a")==0){//判断用户输入
          
            if(i==(sh_num-1)){//如果不让i==sh_num，程序就读不出来段字符n串表的偏移量，无法输出段名
                cout<<"\t索引\t"<<setw(20)<<"名称"<<setw(14)<<"大小"<<setw(13)<<right<<"偏移"<<setw(14)<<"虚拟地址"<<setw(20)<<"类型"<<endl;
                for (int k = 0; k < sh_num; k++)//输出各个段的信息
                {
                    cout<<setw(10)<<right<<k<<setw(25)<<right<<read_sshstr(elf_shstroff,aarr[k],path)<<setw(10)
                    <<right<<arr_size[k]<<setw(10)<<right<<arr_off[k]<<setw(10)<<arr_add[k]<<"\t"<<setw(70)<<left<<(arr_type[k]==SHT_NULL?"无效段":(arr_type[k]==SHT_PROGBITS
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
       
        
    for (int i = 1; i < sh_num; i++)//一定要让i=1,因为arr[0]=‘\0'，会让程序一直输出空字符
    {  
        
        inFile.seekg(elf_shstroff.shstroff,ios::beg);//定位段字符串表的起始位置
        inFile.seekg(arr[i],ios::cur);
        char str[50];
        memset(str,'\0',sizeof(str));//必须清空数组
        inFile.get(str,50,'\0');
        if(strcmp(arg, "a")==0||strcmp(arg, "shs")==0){

            for(int i =0;i<sizeof(str);i++){
                cout<<str[i];
            }
            cout<<endl;
        }
       
    }

    cout<<"***************************************************"<<endl;
    //另一种方式输出
    inFile.seekg(elf_shstroff.shstroff,ios::beg);//定位段字符串表的起始位置
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
         
        inFile.seekg(1,ios::cur);//第一个字符为‘\0’,需要越过
        char str[50];
        memset(str,'\0',sizeof(str));//清空数组
        inFile.get(str,50,'\0');
        for(int i =0;i<sizeof(str);i++){
            cout<<str[i];
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
    cout<<"\t索引"<<right<<setw(20)<<"名称"<<setw(20)<<"大小"<<setw(12)<<"所在段"<<setw(10)<<"符号值"<<endl;
    while (1) {
        
        inFile.read((char *)&elfsym, sizeof(elfsym));
        cout<<setw(10)<<index<<right<<setw(20)<<right<<((int)(elfsym.st_info&0x0000000f)==STT_SECTION?
        "STT_SECTION":read_str(elf_shstroff,path,elfsym.st_name))
        <<right<<setw(15)<<elfsym.st_size<<setw(10)<<right<<elfsym.st_shndx
        <<setw(8)<<right<<elfsym.st_value<<endl;//输出格式
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
    
    if(strcmp(argv[1], "h")==0){//读取elf头
        
        header_in(elf_Header,argv[1],argv[2]);//读取并输出头文件

    }
    else if(strcmp(argv[1], "s")==0){ //打印段表

        header_in(elf_Header,argv[1],argv[2]);
        //为段表做读取准备
        int arr[elf_Header.e_shnum];//用来存储段的名字在段字符串中的偏移
        readSH(elfsh,elf_shstroff,elf_Header.e_shoff,elf_Header.e_shnum,elf_Header.e_shstrndx,arr,
        argv[1],argv[2]);
    }
    else if(strcmp(argv[1], "shs")==0){//打印段字符串
        header_in(elf_Header,argv[1],argv[2]);
        //为段表做读取准备

        int arr[elf_Header.e_shnum];//用来存储段的名字在段字符串中的偏移
        readSH(elfsh,elf_shstroff,elf_Header.e_shoff,elf_Header.e_shnum,elf_Header.e_shstrndx,arr,argv[1],argv[2]);
        //读取字符串
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