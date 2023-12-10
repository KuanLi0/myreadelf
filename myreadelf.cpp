#include<iostream>
#include<fstream>
#include<iomanip>
#include<elf.h>
#include<string.h>
using namespace std;

typedef struct
{
  int symboloff;//记录符号表的偏移量
  int stroff;//记录字符串表的偏移量
  int shstroff;//记录段字符串表的偏移量。
  int symbolsize;//符号表的大小
  int strtabsize;//字符串表的大小
} SH_STR_OFF;//偏移量与符号表、字符串表的大小



Elf64_Shdr elfsh;
Elf64_Sym  elfsym;//符号表结构体

//读取header
Elf64_Ehdr header_in(Elf64_Ehdr & elf){

    ifstream inFile("main.o", ios::in | ios::binary);
    if(! inFile){
        cout   <<"打开文件失败！"<<endl;

    }

    inFile.read((char *)&elf, sizeof(elf));
    cout<<"elf文件类型：\t\t\t\t"<<(elf.e_type==ET_REL?"可重定位":
    (elf.e_type==ET_EXEC?"可执行文件":"共享目标文件"))<<endl;
    cout<<"elf文件段字符串表在段中的下标：\t\t"<<elf.e_shstrndx<<endl;
    cout<<"elf文件入口地址：\t\t\t"<<elf.e_entry<<endl;
    cout<<"elf文件段表在文件中的偏移：\t\t"<<elf.e_shoff<<endl;
    cout<<"elf文件段表描述符的数量：\t\t"<<elf.e_shnum<<endl;//结构体数组有多少个数组。
    cout<<"elf文件属性：\t\t\t\t"<<elf.e_flags<<endl;

    inFile.close();
    return elf;
}


//读取段表
SH_STR_OFF readSH(Elf64_Shdr &elfsh,SH_STR_OFF &elf_shoff,int sh_off,int sh_num,int str_ind,int aarr[]){
    cout<<"*******************打印段表*****************"<<endl;
    cout<<"*******************打印段表*****************"<<endl;

    ifstream inFile("main.o", ios::in | ios::binary);
    if(! inFile){
        cout   <<"打开文件失败！"<<endl;

    }
    inFile.seekg(sh_off,ios::beg);
    

      for (int i = 0; i < sh_num; i++)//用for循环依次拿取
    {   
         inFile.read((char *)&elfsh, sizeof(elfsh));
         if(elfsh.sh_type==SHT_SYMTAB){

            elf_shoff.symboloff=elfsh.sh_offset;//记录符号表的偏移量
            elf_shoff.symbolsize=elfsh.sh_size;//记录符号表的大小
              cout<< elf_shoff.symboloff<<endl;
         }
         if(elfsh.sh_type==SHT_STRTAB&&i!=str_ind){

            elf_shoff.stroff=elfsh.sh_offset;//记录字符串表的偏移量
            elf_shoff.strtabsize=elfsh.sh_size;//记录字符串表的大小
              cout<<elf_shoff.stroff<<endl;
         }
         if(i==str_ind){

            elf_shoff.shstroff=elfsh.sh_offset;//记录段字符串表的偏移量。
            cout<<elf_shoff.shstroff<<endl;
         }

         cout<<"段"<<i<<"的名称：\t"<<elfsh.sh_name<<endl;
         cout<<"段"<<i<<"的大小：\t"<<elfsh.sh_size<<endl;
         cout<<"段"<<i<<"的偏移：\t"<<elfsh.sh_offset<<endl;
         cout<<"段"<<i<<"的虚拟地址：\t"<<elfsh.sh_addr<<endl;
         cout<<"段"<<i<<"的类型：\t"<<(elfsh.sh_type==SHT_NULL?"无效段":(elfsh.sh_type==SHT_PROGBITS
         ?"程序段、代码段、数据段":(elfsh.sh_type==SHT_STRTAB?"字符串表"
         :(elfsh.sh_type==SHT_SYMTAB?"符号表":"其他类型"))))<<endl;
        aarr[i]=elfsh.sh_name;
         
         cout<<"*****************************************\t"<<endl;
    }
    inFile.close();
    
    cout<<"*******************打印段表完毕*****************"<<endl;
    return elf_shoff;
}

//读取段字符串表
void read_shstr(SH_STR_OFF elf_shstroff,int arr[],int sh_num){
    cout<<"*******************打印n段字符串表*****************"<<endl;
    cout<<"*******************打印n段字符串表*****************"<<endl;

    ifstream inFile("main.o", ios::in | ios::binary);
    if(! inFile){
        cout   <<"打开文件失败！"<<endl;

    }
       
     for (int i = 0; i < sh_num; i++)
     {
        inFile.seekg(elf_shstroff.shstroff+1,ios::beg);//定位到刚开始
        inFile.seekg(arr[i],ios::cur);
        char str[50];
        memset(str,'\0',sizeof(str));//清空数组
        inFile.get(str,50,'\0');
        
        for(int i =0;i<sizeof(str);i++){
            cout<<str[i];//输出的时候少个点。
        }
        cout<<endl;
    }
    inFile.close();
    cout<<"*******************打印段字符串表完毕*****************"<<endl;

}

//读取字符串表
void read_shstr(SH_STR_OFF elf_shstroff){
    cout<<"*******************打印字符串表*****************"<<endl;
    cout<<"*******************打印字符串表*****************"<<endl;

    ifstream inFile("main.o", ios::in | ios::binary);
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

//读取符号表
void read_symbol(SH_STR_OFF elf_shstroff){
    cout<<"*******************打印符号表*****************"<<endl;
    cout<<"*******************打印符号表*****************"<<endl;
    ifstream inFile("main.o", ios::in | ios::binary);
    if(! inFile){
        cout   <<"打开文件失败！"<<endl;

    }
    inFile.seekg(elf_shstroff.symboloff,ios::beg);//定位到刚开始
    
    int index=0;//记录符号的序号
       while (1) {
         
            inFile.read((char *)&elfsym, sizeof(elfsym));
            cout<<"符号"<<index<<"的名称：\t"<<elfsym.st_name<<endl;
            cout<<"符号"<<index<<"的大小：\t"<<elfsym.st_size<<endl;
            cout<<"符号"<<index<<"的所在段：\t"<<elfsym.st_shndx<<endl;
            cout<<"符号"<<index<<"的值：\t"<<elfsym.st_value<<endl;
            index++;
            if(inFile.tellg()>=elf_shstroff.symboloff+elf_shstroff.symbolsize-1){
                break;
            }
      }
    inFile.close();
    cout<<"*******************打印符号表完毕*****************"<<endl;
}
int main(){
    Elf64_Ehdr elf_Header;//用来接受elf header
    elf_Header =header_in(elf_Header);//读取并输出头文件

    //为段表做读取准备
    int sh_offnum=elf_Header.e_shoff;//表示段表在文件中的偏移量
    int sh_num=elf_Header.e_shnum;//记录段表结构体数组有多少个数组。
    int sh_strindex=elf_Header.e_shstrndx;//记录文件段字符串表在段中的下标
   
    SH_STR_OFF elf_shstroff;//为读取字符串表和符号表h做准备
     int arr[sh_num];//用来存储段的名字在段字符串中的偏移
    elf_shstroff=readSH(elfsh,elf_shstroff,sh_offnum,sh_num,sh_strindex,arr);
    

    //读取段字符串表
    read_shstr(elf_shstroff,arr,sh_num);

    //读取字符串表
    read_shstr(elf_shstroff);

    read_symbol(elf_shstroff);
    system("pause");
    return 0;
}