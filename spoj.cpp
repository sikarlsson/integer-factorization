#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <stdint.h>
#include <string>
namespace ttmath{typedef unsigned int uint;typedef signed int sint;typedef uint64_t ulint;typedef int64_t slint;}namespace ttmath{enum LibTypeCode{asm_vc_32=0,asm_gcc_32,asm_vc_64,asm_gcc_64,no_asm_32,no_asm_64};enum ErrorCode{err_ok=0,err_nothing_has_read,err_unknown_character,err_unexpected_final_bracket,err_stack_not_clear,err_unknown_variable,err_division_by_zero,err_interrupt,err_overflow,err_unknown_function,err_unknown_operator,err_unexpected_semicolon_operator,err_improper_amount_of_arguments,err_improper_argument,err_unexpected_end,err_internal_error,err_incorrect_name,err_incorrect_value,err_variable_exists,err_variable_loop,err_functions_loop,err_must_be_only_one_value,err_object_exists,err_unknown_object,err_still_calculating,err_in_short_form_used_function,err_percent_from};struct Conv{uint base;bool scient;sint scient_from;bool base_round;sint round;bool trim_zeroes;uint comma;uint comma2;uint group;uint group_digits;uint group_exp;Conv(){base=10;scient=false;scient_from=15;base_round=true;round=-1;trim_zeroes=true;comma='.';comma2=',';group=0;group_digits=3;group_exp=0;}};class StopCalculating{public:virtual bool WasStopSignal()const volatile{return false;}virtual~StopCalculating(){}};class ExceptionInfo{const char*file;int line;public:ExceptionInfo():file(0),line(0){}ExceptionInfo(const char*f,int l):file(f),line(l){}std::string Where()const{if(!file)return"unknown";std::ostringstream result;result<<file<<":"<<line;return result.str();}};class ReferenceError:public std::logic_error,public ExceptionInfo{public:ReferenceError():std::logic_error("reference error"){}ReferenceError(const char*f,int l):std::logic_error("reference error"),ExceptionInfo(f,l){}std::string Where()const{return ExceptionInfo::Where();}};class RuntimeError:public std::runtime_error,public ExceptionInfo{public:RuntimeError():std::runtime_error("internal error"){}RuntimeError(const char*f,int l):std::runtime_error("internal error"),ExceptionInfo(f,l){}std::string Where()const{return ExceptionInfo::Where();}};}namespace ttmath{class Misc{public:static void AssignString(std::string&result,const char*str){result=str;}static void AssignString(std::wstring&result,const char*str){result.clear();for(;*str;++str)result+=*str;}static void AssignString(std::wstring&result,const std::string&str){return AssignString(result,str.c_str());}static void AssignString(std::string&result,const wchar_t*str){result.clear();for(;*str;++str)result+=static_cast<char>(*str);}static void AssignString(std::string&result,const std::wstring&str){return AssignString(result,str.c_str());}static void AddString(std::string&result,const char*str){result+=str;}static void AddString(std::wstring&result,const char*str){for(;*str;++str)result+=*str;}template<class char_type>static void SkipWhiteCharacters(const char_type*&c){while((*c==' ')||(*c=='\t')||(*c==13)||(*c=='\n'))++c;}static uint CharToDigit(uint c){if(c>='0'&&c<='9')return c-'0';if(c>='a'&&c<='z')return c-'a'+10;return c-'A'+10;}static sint CharToDigit(uint c,uint base){if(c>='0'&&c<='9')c=c-'0';else if(c>='a'&&c<='z')c=c-'a'+10;else if(c>='A'&&c<='Z')c=c-'A'+10;else return-1;if(c>=base)return-1;return sint(c);}static uint DigitToChar(uint digit){if(digit<10)return digit+'0';return digit-10+'A';}};}namespace ttmath{template<uint value_size>class UInt{public:uint table[value_size];template<class ostream_type>void PrintTable(ostream_type&output)const{const int columns=8;int c=1;for(int i=value_size-1;i>=0;--i){output<<"0x"<<std::setfill('0');output<<std::setw(8);output<<std::hex<<table[i];if(i>0){output<<",";if(++c>columns){output<<std::endl;c=1;}}}output<<std::dec<<std::endl;}template<class char_type,class ostream_type>static void PrintVectorLog(const char_type*msg,ostream_type&output,const uint*vector,uint vector_len){output<<msg<<std::endl;for(uint i=0;i<vector_len;++i)output<<"table["<<i<<"]:"<<vector[i]<<std::endl;}template<class char_type,class ostream_type>static void PrintVectorLog(const char_type*msg,uint carry,ostream_type&output,const uint*vector,uint vector_len){PrintVectorLog(msg,output,vector,vector_len);output<<"carry:"<<carry<<std::endl;}template<class char_type,class ostream_type>void PrintLog(const char_type*msg,ostream_type&output)const{PrintVectorLog(msg,output,table,value_size);}template<class char_type,class ostream_type>void PrintLog(const char_type*msg,uint carry,ostream_type&output)const{PrintVectorLog(msg,output,table,value_size);output<<"carry:"<<carry<<std::endl;}uint Size()const{return value_size;}void SetZero(){for(uint i=0;i<value_size;++i)table[i]=0;}void SetOne(){SetZero();table[0]=1;}void SetMax(){for(uint i=0;i<value_size;++i)table[i]=4294967295u;}void SetMin(){SetZero();}void Swap(UInt<value_size>&ss2){for(uint i=0;i<value_size;++i){uint temp=table[i];table[i]=ss2.table[i];ss2.table[i]=temp;}}void SetFromTable(const uint*temp_table,uint temp_table_len){uint temp_table_index=0;sint i;for(i=value_size-1;i>=0&&temp_table_index<temp_table_len;--i,++temp_table_index)table[i]=temp_table[temp_table_index];if(temp_table_index<temp_table_len){if((temp_table[temp_table_index]&2147483648u)!=0){if(table[0]!=4294967295u)++table[0];}}for(;i>=0;--i)table[i]=0;}uint AddOne(){return AddInt(1);}uint SubOne(){return SubInt(1);}private:void RclMoveAllWords(uint&rest_bits,uint&last_c,uint bits,uint c){rest_bits=bits %32u;uint all_words=bits/32u;uint mask=(c)?4294967295u:0;if(all_words>=value_size){if(all_words==value_size&&rest_bits==0)last_c=table[0]&1;for(uint i=0;i<value_size;++i)table[i]=mask;rest_bits=0;}else if(all_words>0){sint first,second;last_c=table[value_size-all_words]&1;for(first=value_size-1,second=first-all_words;second>=0;--first,--second)table[first]=table[second];for(;first>=0;--first)table[first]=mask;}}public:uint Rcl(uint bits,uint c=0){uint last_c=0;uint rest_bits=bits;if(bits==0)return 0;if(bits>=32u)RclMoveAllWords(rest_bits,last_c,bits,c);if(rest_bits==0){return last_c;}if(rest_bits==1){last_c=Rcl2_one(c);}else if(rest_bits==2){Rcl2_one(c);last_c=Rcl2_one(c);}else{last_c=Rcl2(rest_bits,c);}return last_c;}private:void RcrMoveAllWords(uint&rest_bits,uint&last_c,uint bits,uint c){rest_bits=bits %32u;uint all_words=bits/32u;uint mask=(c)?4294967295u:0;if(all_words>=value_size){if(all_words==value_size&&rest_bits==0)last_c=(table[value_size-1]&2147483648u)?1:0;for(uint i=0;i<value_size;++i)table[i]=mask;rest_bits=0;}else if(all_words>0){uint first,second;last_c=(table[all_words-1]&2147483648u)?1:0;for(first=0,second=all_words;second<value_size;++first,++second)table[first]=table[second];for(;first<value_size;++first)table[first]=mask;}}public:uint Rcr(uint bits,uint c=0){uint last_c=0;uint rest_bits=bits;if(bits==0)return 0;if(bits>=32u)RcrMoveAllWords(rest_bits,last_c,bits,c);if(rest_bits==0){return last_c;}if(rest_bits==1){last_c=Rcr2_one(c);}else if(rest_bits==2){Rcr2_one(c);last_c=Rcr2_one(c);}else{last_c=Rcr2(rest_bits,c);}return last_c;}uint CompensationToLeft(){uint moving=0;sint a;for(a=value_size-1;a>=0&&table[a]==0;--a);if(a<0)return moving;if(a!=value_size-1){moving+=(value_size-1-a)*32u;sint i;for(i=value_size-1;a>=0;--i,--a)table[i]=table[a];for(;i>=0;--i)table[i]=0;}uint moving2=FindLeadingBitInWord(table[value_size-1]);moving2=32u-moving2-1;Rcl(moving2);return moving+moving2;}bool FindLeadingBit(uint&table_id,uint&index)const{for(table_id=value_size-1;table_id!=0&&table[table_id]==0;--table_id);if(table_id==0&&table[table_id]==0){index=0;return false;}index=FindLeadingBitInWord(table[table_id]);return true;}bool FindLowestBit(uint&table_id,uint&index)const{for(table_id=0;table_id<value_size&&table[table_id]==0;++table_id);if(table_id>=value_size){index=0;table_id=0;return false;}index=FindLowestBitInWord(table[table_id]);return true;}uint GetBit(uint bit_index)const{uint index=bit_index/32u;uint bit=bit_index %32u;uint temp=table[index];uint res=SetBitInWord(temp,bit);return res;}uint SetBit(uint bit_index){uint index=bit_index/32u;uint bit=bit_index %32u;uint res=SetBitInWord(table[index],bit);return res;}void BitAnd(const UInt<value_size>&ss2){for(uint x=0;x<value_size;++x)table[x]&=ss2.table[x];}void BitOr(const UInt<value_size>&ss2){for(uint x=0;x<value_size;++x)table[x]|=ss2.table[x];}void BitXor(const UInt<value_size>&ss2){for(uint x=0;x<value_size;++x)table[x]^=ss2.table[x];}void BitNot(){for(uint x=0;x<value_size;++x)table[x]=~table[x];}void BitNot2(){uint table_id,index;if(FindLeadingBit(table_id,index)){for(uint x=0;x<table_id;++x)table[x]=~table[x];uint mask=4294967295u;uint shift=32u-index-1;if(shift)mask>>=shift;table[table_id]^=mask;}else table[0]=1;}public:uint MulInt(uint ss2){uint r1,r2,x1;uint c=0;UInt<value_size>u(*this);SetZero();if(ss2==0){return 0;}for(x1=0;x1<value_size-1;++x1){MulTwoWords(u.table[x1],ss2,&r2,&r1);c+=AddTwoInts(r2,r1,x1);}MulTwoWords(u.table[x1],ss2,&r2,&r1);c+=(r2!=0)?1:0;c+=AddInt(r1,x1);return (c==0)?0:1;}template<uint result_size>void MulInt(uint ss2,UInt<result_size>&result)const{uint r2,r1;uint x1size=value_size;uint x1start=0;result.SetZero();if(ss2==0){return;}if(value_size>2){for(x1size=value_size;x1size>0&&table[x1size-1]==0;--x1size);if(x1size==0){return;}for(x1start=0;x1start<x1size&&table[x1start]==0;++x1start);}for(uint x1=x1start;x1<x1size;++x1){MulTwoWords(table[x1],ss2,&r2,&r1);result.AddTwoInts(r2,r1,x1);}return;}uint Mul(const UInt<value_size>&ss2,uint algorithm=100){switch(algorithm){case 1:return Mul1(ss2);case 2:return Mul2(ss2);case 3:return Mul3(ss2);case 100:default:return MulFastest(ss2);}}void MulBig(const UInt<value_size>&ss2,UInt<value_size*2>&result,uint algorithm=100){switch(algorithm){case 1:return Mul1Big(ss2,result);case 2:return Mul2Big(ss2,result);case 3:return Mul3Big(ss2,result);case 100:default:return MulFastestBig(ss2,result);}}private:uint Mul1Ref(const UInt<value_size>&ss2){UInt<value_size>ss1(*this);SetZero();for(uint i=0;i<value_size*32u;++i){if(Add(*this)){return 1;}if(ss1.Rcl(1))if(Add(ss2)){return 1;}}return 0;}public:uint Mul1(const UInt<value_size>&ss2){if(this==&ss2){UInt<value_size>copy_ss2(ss2);return Mul1Ref(copy_ss2);}else{return Mul1Ref(ss2);}}void Mul1Big(const UInt<value_size>&ss2_,UInt<value_size*2>&result){UInt<value_size*2>ss2;uint i;for(i=0;i<value_size;++i){result.table[i]=table[i];ss2.table[i]=ss2_.table[i];}for(;i<value_size*2;++i){result.table[i]=0;ss2.table[i]=0;}result.Mul1(ss2);}uint Mul2(const UInt<value_size>&ss2){UInt<value_size*2>result;uint i,c=0;Mul2Big(ss2,result);for(i=0;i<value_size;++i)table[i]=result.table[i];for(;i<value_size*2;++i)if(result.table[i]!=0){c=1;break;}return c;}void Mul2Big(const UInt<value_size>&ss2,UInt<value_size*2>&result){Mul2Big2<value_size>(table,ss2.table,result);}private:template<uint ss_size>void Mul2Big2(const uint*ss1,const uint*ss2,UInt<ss_size*2>&result){uint x1size=ss_size,x2size=ss_size;uint x1start=0,x2start=0;if(ss_size>2){for(x1size=ss_size;x1size>0&&ss1[x1size-1]==0;--x1size);for(x2size=ss_size;x2size>0&&ss2[x2size-1]==0;--x2size);for(x1start=0;x1start<x1size&&ss1[x1start]==0;++x1start);for(x2start=0;x2start<x2size&&ss2[x2start]==0;++x2start);}Mul2Big3<ss_size>(ss1,ss2,result,x1start,x1size,x2start,x2size);}template<uint ss_size>void Mul2Big3(const uint*ss1,const uint*ss2,UInt<ss_size*2>&result,uint x1start,uint x1size,uint x2start,uint x2size){uint r2,r1;result.SetZero();if(x1size==0||x2size==0)return;for(uint x1=x1start;x1<x1size;++x1){for(uint x2=x2start;x2<x2size;++x2){MulTwoWords(ss1[x1],ss2[x2],&r2,&r1);result.AddTwoInts(r2,r1,x2+x1);}}}public:uint Mul3(const UInt<value_size>&ss2){UInt<value_size*2>result;uint i,c=0;Mul3Big(ss2,result);for(i=0;i<value_size;++i)table[i]=result.table[i];for(;i<value_size*2;++i)if(result.table[i]!=0){c=1;break;}return c;}void Mul3Big(const UInt<value_size>&ss2,UInt<value_size*2>&result){Mul3Big2<value_size>(table,ss2.table,result.table);}private:template<uint ss_size>void Mul3Big2(const uint*ss1,const uint*ss2,uint*result){const uint*x1,*x0,*y1,*y0;if(ss_size>1&&ss_size<3){UInt<ss_size*2>res;Mul2Big2<ss_size>(ss1,ss2,res);for(uint i=0;i<ss_size*2;++i)result[i]=res.table[i];return;}else if(ss_size==1){return MulTwoWords(*ss1,*ss2,&result[1],&result[0]);}if((ss_size&1)==1){x0=ss1;y0=ss2;x1=ss1+ss_size/2+1;y1=ss2+ss_size/2+1;Mul3Big3<ss_size/2+1,ss_size/2,ss_size*2>(x1,x0,y1,y0,result);}else{x0=ss1;y0=ss2;x1=ss1+ss_size/2;y1=ss2+ss_size/2;Mul3Big3<ss_size/2,ss_size/2,ss_size*2>(x1,x0,y1,y0,result);}}template<uint first_size,uint second_size,uint result_size>void Mul3Big3(const uint*x1,const uint*x0,const uint*y1,const uint*y0,uint*result){uint i,c,xc,yc;UInt<first_size>temp,temp2;UInt<first_size*3>z1;Mul3Big2<first_size>(x0,y0,result);Mul3Big2<second_size>(x1,y1,result+first_size*2);xc=AddVector(x0,x1,first_size,second_size,temp.table);yc=AddVector(y0,y1,first_size,second_size,temp2.table);Mul3Big2<first_size>(temp.table,temp2.table,z1.table);for(i=first_size*2;i<first_size*3;++i)z1.table[i]=0;if(xc){c=AddVector(z1.table+first_size,temp2.table,first_size*3-first_size,first_size,z1.table+first_size);}if(yc){c=AddVector(z1.table+first_size,temp.table,first_size*3-first_size,first_size,z1.table+first_size);}if(xc&&yc){for(i=first_size*2;i<first_size*3;++i)if(++z1.table[i]!=0)break;}c=SubVector(z1.table,result+first_size*2,first_size*3,second_size*2,z1.table);c=SubVector(z1.table,result,first_size*3,first_size*2,z1.table);if(first_size>second_size){uint z1_size=result_size-first_size;for(i=z1_size;i<first_size*3;++i){}c=AddVector(result+first_size,z1.table,result_size-first_size,z1_size,result+first_size);}else{c=AddVector(result+first_size,z1.table,result_size-first_size,first_size*3,result+first_size);}}public:uint MulFastest(const UInt<value_size>&ss2){UInt<value_size*2>result;uint i,c=0;MulFastestBig(ss2,result);for(i=0;i<value_size;++i)table[i]=result.table[i];for(;i<value_size*2;++i)if(result.table[i]!=0){c=1;break;}return c;}void MulFastestBig(const UInt<value_size>&ss2,UInt<value_size*2>&result){if(value_size<3)return Mul2Big(ss2,result);uint x1size=value_size,x2size=value_size;uint x1start=0,x2start=0;for(x1size=value_size;x1size>0&&table[x1size-1]==0;--x1size);for(x2size=value_size;x2size>0&&ss2.table[x2size-1]==0;--x2size);if(x1size==0||x2size==0){result.SetZero();return;}for(x1start=0;x1start<x1size&&table[x1start]==0;++x1start);for(x2start=0;x2start<x2size&&ss2.table[x2start]==0;++x2start);uint distancex1=x1size-x1start;uint distancex2=x2size-x2start;if(distancex1<3||distancex2<3)return Mul2Big3<value_size>(table,ss2.table,result,x1start,x1size,x2start,x2size);Mul3Big(ss2,result);}public:uint DivInt(uint divisor,uint*remainder=0){if(divisor==0){if(remainder)*remainder=0;return 1;}if(divisor==1){if(remainder)*remainder=0;return 0;}UInt<value_size>dividend(*this);SetZero();sint i;uint r=0;for(i=value_size-1;i>0&&dividend.table[i]==0;--i);for(;i>=0;--i)DivTwoWords(r,dividend.table[i],divisor,&table[i],&r);if(remainder)*remainder=r;return 0;}uint DivInt(uint divisor,uint&remainder){return DivInt(divisor,&remainder);}uint Div(const UInt<value_size>&divisor,UInt<value_size>*remainder=0,uint algorithm=3){switch(algorithm){case 1:return Div1(divisor,remainder);case 2:return Div2(divisor,remainder);case 3:default:return Div3(divisor,remainder);}}uint Div(const UInt<value_size>&divisor,UInt<value_size>&remainder,uint algorithm=3){return Div(divisor,&remainder,algorithm);}private:uint Div_StandardTest(const UInt<value_size>&v,uint&m,uint&n,UInt<value_size>*remainder=0){switch(Div_CalculatingSize(v,m,n)){case 4:if(remainder)remainder->SetZero();SetOne();return 0;case 3:if(remainder)*remainder=*this;SetZero();return 0;case 2:if(remainder)remainder->SetZero();SetZero();return 0;case 1:return 1;}return 2;}uint Div_CalculatingSize(const UInt<value_size>&v,uint&m,uint&n){m=n=value_size-1;for(;n!=0&&v.table[n]==0;--n);if(n==0&&v.table[n]==0)return 1;for(;m!=0&&table[m]==0;--m);if(m==0&&table[m]==0)return 2;if(m<n)return 3;else if(m==n){uint i;for(i=n;i!=0&&table[i]==v.table[i];--i);if(table[i]<v.table[i])return 3;else if (table[i]==v.table[i])return 4;}return 0;}public:uint Div1(const UInt<value_size>&divisor,UInt<value_size>*remainder=0){uint m,n,test;test=Div_StandardTest(divisor,m,n,remainder);if(test<2)return test;if(!remainder){UInt<value_size>rem;return Div1_Calculate(divisor,rem);}return Div1_Calculate(divisor,*remainder);}uint Div1(const UInt<value_size>&divisor,UInt<value_size>&remainder){return Div1(divisor,&remainder);}private:uint Div1_Calculate(const UInt<value_size>&divisor,UInt<value_size>&rest){if(this==&divisor){UInt<value_size>divisor_copy(divisor);return Div1_CalculateRef(divisor_copy,rest);}else{return Div1_CalculateRef(divisor,rest);}}uint Div1_CalculateRef(const UInt<value_size>&divisor,UInt<value_size>&rest){sint loop;sint c;rest.SetZero();loop=value_size*32u;c=0;div_a:c=Rcl(1,c);c=rest.Add(rest,c);c=rest.Sub(divisor,c);c=!c;if(!c)goto div_d;div_b:--loop;if(loop)goto div_a;c=Rcl(1,c);return 0;div_c:c=Rcl(1,c);c=rest.Add(rest,c);c=rest.Add(divisor);if(c)goto div_b;div_d:--loop;if(loop)goto div_c;c=Rcl(1,c);c=rest.Add(divisor);return 0;}public:uint Div2(const UInt<value_size>&divisor,UInt<value_size>*remainder=0){if(this==&divisor){UInt<value_size>divisor_copy(divisor);return Div2Ref(divisor_copy,remainder);}else{return Div2Ref(divisor,remainder);}}uint Div2(const UInt<value_size>&divisor,UInt<value_size>&remainder){return Div2(divisor,&remainder);}private:uint Div2Ref(const UInt<value_size>&divisor,UInt<value_size>*remainder=0){uint bits_diff;uint status=Div2_Calculate(divisor,remainder,bits_diff);if(status<2)return status;if(CmpBiggerEqual(divisor)){Div2(divisor,remainder);SetBit(bits_diff);}else{if(remainder)*remainder=*this;SetZero();SetBit(bits_diff);}return 0;}uint Div2_Calculate(const UInt<value_size>&divisor,UInt<value_size>*remainder,uint&bits_diff){uint table_id,index;uint divisor_table_id,divisor_index;uint status=Div2_FindLeadingBitsAndCheck(divisor,remainder,table_id,index,divisor_table_id,divisor_index);if(status<2){return status;}bits_diff=index-divisor_index;UInt<value_size>divisor_copy(divisor);divisor_copy.Rcl(bits_diff,0);if(CmpSmaller(divisor_copy,table_id)){divisor_copy.Rcr(1);--bits_diff;}Sub(divisor_copy,0);return 2;}uint Div2_FindLeadingBitsAndCheck(const UInt<value_size>&divisor,UInt<value_size>*remainder,uint&table_id,uint&index,uint&divisor_table_id,uint&divisor_index){if(!divisor.FindLeadingBit(divisor_table_id,divisor_index)){return 1;}if(!FindLeadingBit(table_id,index)){SetZero();if(remainder)remainder->SetZero();return 0;}divisor_index+=divisor_table_id*32u;index+=table_id*32u;if(divisor_table_id==0){uint r;DivInt(divisor.table[0],&r);if(remainder){remainder->SetZero();remainder->table[0]=r;}return 0;}if(Div2_DivisorGreaterOrEqual(divisor,remainder,table_id,index,divisor_index)){return 0;}return 2;}bool Div2_DivisorGreaterOrEqual(const UInt<value_size>&divisor,UInt<value_size>*remainder,uint table_id,uint index,uint divisor_index){if(divisor_index>index){if(remainder)*remainder=*this;SetZero();return true;}if(divisor_index==index){uint i;for(i=table_id;i!=0&&table[i]==divisor.table[i];--i);if(table[i]<divisor.table[i]){if(remainder)*remainder=*this;SetZero();return true;}else if(table[i]==divisor.table[i]){if(remainder)remainder->SetZero();SetOne();return true;}}return false;}public:uint Div3(const UInt<value_size>&ss2,UInt<value_size>*remainder=0){if(this==&ss2){UInt<value_size>copy_ss2(ss2);return Div3Ref(copy_ss2,remainder);}else{return Div3Ref(ss2,remainder);}}uint Div3(const UInt<value_size>&ss2,UInt<value_size>&remainder){return Div3(ss2,&remainder);}private:uint Div3Ref(const UInt<value_size>&v,UInt<value_size>*remainder=0){uint m,n,test;test=Div_StandardTest(v,m,n,remainder);if(test<2)return test;if(n==0){uint r;DivInt(v.table[0],&r);if(remainder){remainder->SetZero();remainder->table[0]=r;}return 0;}++m;++n;m=m-n;Div3_Division(v,remainder,m,n);return 0;}private:void Div3_Division(UInt<value_size>v,UInt<value_size>*remainder,uint m,uint n){UInt<value_size+1>uu,vv;UInt<value_size>q;uint d,u_value_size,u0,u1,u2,v1,v0,j=m;u_value_size=Div3_Normalize(v,n,d);if(j+n==value_size)u2=u_value_size;else u2=table[j+n];Div3_MakeBiggerV(v,vv);for(uint i=j+1;i<value_size;++i)q.table[i]=0;while(true){u1=table[j+n-1];u0=table[j+n-2];v1=v.table[n-1];v0=v.table[n-2];uint qp=Div3_Calculate(u2,u1,u0,v1,v0);Div3_MakeNewU(uu,j,n,u2);Div3_MultiplySubtract(uu,vv,qp);Div3_CopyNewU(uu,j,n);q.table[j]=qp;if(j--==0)break;u2=table[j+n];}if(remainder)Div3_Unnormalize(remainder,n,d);*this=q;}void Div3_MakeNewU(UInt<value_size+1>&uu,uint j,uint n,uint u_max){uint i;for(i=0;i<n;++i,++j)uu.table[i]=table[j];uu.table[i]=u_max;for(++i;i<value_size+1;++i)uu.table[i]=0;}void Div3_CopyNewU(const UInt<value_size+1>&uu,uint j,uint n){uint i;for(i=0;i<n;++i)table[i+j]=uu.table[i];if(i+j<value_size)table[i+j]=uu.table[i];}void Div3_MakeBiggerV(const UInt<value_size>&v,UInt<value_size+1>&vv){for(uint i=0;i<value_size;++i)vv.table[i]=v.table[i];vv.table[value_size]=0;}uint Div3_Normalize(UInt<value_size>&v,uint n,uint&d){uint bit=(uint)FindLeadingBitInWord(v.table[n-1]);uint move=(32u-bit-1);uint res=table[value_size-1];d=move;if(move>0){v.Rcl(move,0);Rcl(move,0);res=res>>(bit+1);}else{res=0;}return res;}void Div3_Unnormalize(UInt<value_size>*remainder,uint n,uint d){for(uint i=n;i<value_size;++i)table[i]=0;Rcr(d,0);*remainder=*this;}uint Div3_Calculate(uint u2,uint u1,uint u0,uint v1,uint v0){UInt<2>u_temp;uint rp;bool next_test;u_temp.table[1]=u2;u_temp.table[0]=u1;u_temp.DivInt(v1,&rp);do{bool decrease=false;if(u_temp.table[1]==1)decrease=true;else{UInt<2>temp1,temp2;UInt<2>::MulTwoWords(u_temp.table[0],v0,temp1.table+1,temp1.table);temp2.table[1]=rp;temp2.table[0]=u0;if(temp1>temp2)decrease=true;}next_test=false;if(decrease){u_temp.SubOne();rp+=v1;if(rp>=v1)next_test=true;}}while(next_test);return u_temp.table[0];}void Div3_MultiplySubtract(UInt<value_size+1>&uu,const UInt<value_size+1>&vv,uint&qp){UInt<value_size+1>vv_temp(vv);vv_temp.MulInt(qp);if(uu.Sub(vv_temp)){--qp;uu.Add(vv);}}public:uint Pow(UInt<value_size>pow){if(pow.IsZero()&&IsZero())return 2;UInt<value_size>start(*this);UInt<value_size>result;result.SetOne();uint c=0;while(!c){if(pow.table[0]&1)c+=result.Mul(start);pow.Rcr2_one(0);if(pow.IsZero())break;c+=start.Mul(start);}*this=result;return (c==0)?0:1;}void Sqrt(){UInt<value_size>bit,temp;if(IsZero())return;UInt<value_size>value(*this);SetZero();bit.SetZero();bit.table[value_size-1]=(2147483648u>>1);while(bit>value)bit.Rcr(2);while(!bit.IsZero()){temp=*this;temp.Add(bit);if(value>=temp){value.Sub(temp);Rcr(1);Add(bit);}else{Rcr(1);}bit.Rcr(2);}}void ClearFirstBits(uint n){if(n>=value_size*32u){SetZero();return;}uint*p=table;while(n>=32u){*p++=0;n-=32u;}if(n==0){return;}uint mask=4294967295u;mask=mask<<n;(*p)&=mask;}bool IsTheHighestBitSet()const{return (table[value_size-1]&2147483648u)!=0;}bool IsTheLowestBitSet()const{return (*table&1)!=0;}bool IsOnlyTheHighestBitSet()const{for(uint i=0;i<value_size-1;++i)if(table[i]!=0)return false;if(table[value_size-1]!=2147483648u)return false;return true;}bool IsOnlyTheLowestBitSet()const{if(table[0]!=1)return false;for(uint i=1;i<value_size;++i)if(table[i]!=0)return false;return true;}bool IsZero()const{for(uint i=0;i<value_size;++i)if(table[i]!=0)return false;return true;}bool AreFirstBitsZero(uint bits)const{uint index=bits/32u;uint rest=bits %32u;uint i;for(i=0;i<index;++i)if(table[i]!=0)return false;if(rest==0)return true;uint mask=4294967295u>>(32u-rest);return (table[i]&mask)==0;}template<uint argument_size>uint FromUInt(const UInt<argument_size>&p){uint min_size=(value_size<argument_size)?value_size:argument_size;uint i;for(i=0;i<min_size;++i)table[i]=p.table[i];if(value_size>argument_size){for(;i<value_size;++i)table[i]=0;}else{for(;i<argument_size;++i)if(p.table[i]!=0){return 1;}}return 0;}template<uint argument_size>uint FromInt(const UInt<argument_size>&p){return FromUInt(p);}uint FromUInt(uint value){for(uint i=1;i<value_size;++i)table[i]=0;table[0]=value;return 0;}uint FromInt(uint value){return FromUInt(value);}uint FromInt(sint value){uint c=FromUInt(uint(value));if(c||value<0)return 1;return 0;}template<uint argument_size>UInt<value_size>&operator=(const UInt<argument_size>&p){FromUInt(p);return*this;}UInt<value_size>&operator=(const UInt<value_size>&p){for(uint i=0;i<value_size;++i)table[i]=p.table[i];return*this;}UInt<value_size>&operator=(uint i){FromUInt(i);return*this;}UInt(uint i){FromUInt(i);}UInt<value_size>&operator=(sint i){FromInt(i);return*this;}UInt(sint i){FromInt(i);}uint FromUInt(ulint n){table[0]=(uint)n;if(value_size==1){uint c=((n>>32u)==0)?0:1;return c;}table[1]=(uint)(n>>32u);for(uint i=2;i<value_size;++i)table[i]=0;return 0;}uint FromInt(ulint n){return FromUInt(n);}uint FromInt(slint n){uint c=FromUInt(ulint(n));if(c||n<0)return 1;return 0;}UInt<value_size>&operator=(ulint n){FromUInt(n);return*this;}UInt(ulint n){FromUInt(n);}UInt<value_size>&operator=(slint n){FromInt(n);return*this;}UInt(slint n){FromInt(n);}UInt(const char*s){FromString(s);}UInt(const std::string&s){FromString(s.c_str());}UInt(const wchar_t*s){FromString(s);}UInt(const std::wstring&s){FromString(s.c_str());}UInt(){}UInt(const UInt<value_size>&u){for(uint i=0;i<value_size;++i)table[i]=u.table[i];}template<uint argument_size>UInt(const UInt<argument_size>&u){FromUInt(u);}~UInt(){}uint ToUInt()const{return table[0];}uint ToUInt(uint&result)const{result=table[0];for(uint i=1;i<value_size;++i)if(table[i]!=0)return 1;return 0;}uint ToInt(uint&result)const{return ToUInt(result);}uint ToInt(sint&result)const{result=sint(table[0]);if((result&2147483648u)!=0)return 1;for(uint i=1;i<value_size;++i)if(table[i]!=0)return 1;return 0;}uint ToUInt(ulint&result)const{if(value_size==1){result=table[0];}else{uint low=table[0];uint high=table[1];result=low;result|=(ulint(high)<<32u);for(uint i=2;i<value_size;++i)if(table[i]!=0)return 1;}return 0;}uint ToInt(ulint&result)const{return ToUInt(result);}uint ToInt(slint&result)const{ulint temp;uint c=ToUInt(temp);result=slint(temp);if(c||result<0)return 1;return 0;}protected:double ToStringLog2(uint x)const{static double log_tab[]={1.000000000000000000,0.630929753571457437,0.500000000000000000,0.430676558073393050,0.386852807234541586,0.356207187108022176,0.333333333333333333,0.315464876785728718,0.301029995663981195,0.289064826317887859,0.278942945651129843,0.270238154427319741,0.262649535037193547,0.255958024809815489,0.250000000000000000};if(x<2||x>16)return 0;return log_tab[x-2];}public:template<class string_type>void ToStringBase(string_type&result,uint b=10,bool negative=false)const{UInt<value_size>temp(*this);uint rest,table_id,index,digits;double digits_d;char character;result.clear();if(b<2||b>16)return;if(!FindLeadingBit(table_id,index)){result='0';return;}if(negative)result='-';digits_d=table_id;digits_d*=32u;digits_d+=index+1;digits_d*=ToStringLog2(b);digits=static_cast<uint>(digits_d)+3;if(result.capacity()<digits)result.reserve(digits);do{temp.DivInt(b,&rest);character=static_cast<char>(Misc::DigitToChar(rest));result.insert(result.end(),character);}while(!temp.IsZero());size_t i1=negative?1:0;size_t i2=result.size()-1;for(;i1<i2;++i1,--i2){char tempc=static_cast<char>(result[i1]);result[i1]=result[i2];result[i2]=tempc;}}void ToString(std::string&result,uint b=10)const{return ToStringBase(result,b);}std::string ToString(uint b=10)const{std::string result;ToStringBase(result,b);return result;}void ToString(std::wstring&result,uint b=10)const{return ToStringBase(result,b);}std::wstring ToWString(uint b=10)const{std::wstring result;ToStringBase(result,b);return result;}private:template<class char_type>uint FromStringBase(const char_type*s,uint b=10,const char_type**after_source=0,bool*value_read=0){UInt<value_size>base(b);UInt<value_size>temp;sint z;uint c=0;SetZero();temp.SetZero();Misc::SkipWhiteCharacters(s);if(after_source)*after_source=s;if(value_read)*value_read=false;if(b<2||b>16)return 1;for(;(z=Misc::CharToDigit(*s,b))!=-1;++s){if(value_read)*value_read=true;if(c==0){temp.table[0]=z;c+=Mul(base);c+=Add(temp);}}if(after_source)*after_source=s;return (c==0)?0:1;}public:uint FromString(const char*s,uint b=10,const char**after_source=0,bool*value_read=0){return FromStringBase(s,b,after_source,value_read);}uint FromString(const std::string&s,uint b=10){return FromString(s.c_str(),b);}UInt<value_size>&operator=(const char*s){FromString(s);return*this;}UInt<value_size>&operator=(const std::string&s){FromString(s.c_str());return*this;}uint FromString(const wchar_t*s,uint b=10,const wchar_t**after_source=0,bool*value_read=0){return FromStringBase(s,b,after_source,value_read);}uint FromString(const std::wstring&s,uint b=10){return FromString(s.c_str(),b);}UInt<value_size>&operator=(const wchar_t*s){FromString(s);return*this;}UInt<value_size>&operator=(const std::wstring&s){FromString(s.c_str());return*this;}bool CmpSmaller(const UInt<value_size>&l,sint index=-1)const{sint i;if(index==-1||index>=sint(value_size))i=value_size-1;else i=index;for(;i>=0;--i){if(table[i]!=l.table[i])return table[i]<l.table[i];}return false;}bool CmpBigger(const UInt<value_size>&l,sint index=-1)const{sint i;if(index==-1||index>=sint(value_size))i=value_size-1;else i=index;for(;i>=0;--i){if(table[i]!=l.table[i])return table[i]>l.table[i];}return false;}bool CmpEqual(const UInt<value_size>&l,sint index=-1)const{sint i;if(index==-1||index>=sint(value_size))i=value_size-1;else i=index;for(;i>=0;--i)if(table[i]!=l.table[i])return false;return true;}bool CmpSmallerEqual(const UInt<value_size>&l,sint index=-1)const{sint i;if(index==-1||index>=sint(value_size))i=value_size-1;else i=index;for(;i>=0;--i){if(table[i]!=l.table[i])return table[i]<l.table[i];}return true;}bool CmpBiggerEqual(const UInt<value_size>&l,sint index=-1)const{sint i;if(index==-1||index>=sint(value_size))i=value_size-1;else i=index;for(;i>=0;--i){if(table[i]!=l.table[i])return table[i]>l.table[i];}return true;}bool operator<(const UInt<value_size>&l)const{return CmpSmaller(l);}bool operator>(const UInt<value_size>&l)const{return CmpBigger(l);}bool operator==(const UInt<value_size>&l)const{return CmpEqual(l);}bool operator!=(const UInt<value_size>&l)const{return!operator==(l);}bool operator<=(const UInt<value_size>&l)const{return CmpSmallerEqual(l);}bool operator>=(const UInt<value_size>&l)const{return CmpBiggerEqual(l);}UInt<value_size>operator-(const UInt<value_size>&p2)const{UInt<value_size>temp(*this);temp.Sub(p2);return temp;}UInt<value_size>&operator-=(const UInt<value_size>&p2){Sub(p2);return*this;}UInt<value_size>operator+(const UInt<value_size>&p2)const{UInt<value_size>temp(*this);temp.Add(p2);return temp;}UInt<value_size>&operator+=(const UInt<value_size>&p2){Add(p2);return*this;}UInt<value_size>operator*(const UInt<value_size>&p2)const{UInt<value_size>temp(*this);temp.Mul(p2);return temp;}UInt<value_size>&operator*=(const UInt<value_size>&p2){Mul(p2);return*this;}UInt<value_size>operator/(const UInt<value_size>&p2)const{UInt<value_size>temp(*this);temp.Div(p2);return temp;}UInt<value_size>&operator/=(const UInt<value_size>&p2){Div(p2);return*this;}UInt<value_size>operator%(const UInt<value_size>&p2)const{UInt<value_size>temp(*this);UInt<value_size>remainder;temp.Div(p2,remainder);return remainder;}UInt<value_size>&operator%=(const UInt<value_size>&p2){UInt<value_size>remainder;Div(p2,remainder);operator=(remainder);return*this;}UInt<value_size>&operator++(){AddOne();return*this;}UInt<value_size>operator++(int){UInt<value_size>temp(*this);AddOne();return temp;}UInt<value_size>&operator--(){SubOne();return*this;}UInt<value_size>operator--(int){UInt<value_size>temp(*this);SubOne();return temp;}UInt<value_size>operator~()const{UInt<value_size>temp(*this);temp.BitNot();return temp;}UInt<value_size>operator&(const UInt<value_size>&p2)const{UInt<value_size>temp(*this);temp.BitAnd(p2);return temp;}UInt<value_size>&operator&=(const UInt<value_size>&p2){BitAnd(p2);return*this;}UInt<value_size>operator|(const UInt<value_size>&p2)const{UInt<value_size>temp(*this);temp.BitOr(p2);return temp;}UInt<value_size>&operator|=(const UInt<value_size>&p2){BitOr(p2);return*this;}UInt<value_size>operator^(const UInt<value_size>&p2)const{UInt<value_size>temp(*this);temp.BitXor(p2);return temp;}UInt<value_size>&operator^=(const UInt<value_size>&p2){BitXor(p2);return*this;}UInt<value_size>operator>>(int move)const{UInt<value_size>temp(*this);temp.Rcr(move);return temp;}UInt<value_size>&operator>>=(int move){Rcr(move);return*this;}UInt<value_size>operator<<(int move)const{UInt<value_size>temp(*this);temp.Rcl(move);return temp;}UInt<value_size>&operator<<=(int move){Rcl(move);return*this;}private:template<class ostream_type,class string_type>static ostream_type&OutputToStream(ostream_type&s,const UInt<value_size>&l){string_type ss;l.ToString(ss);s<<ss;return s;}public:friend std::ostream&operator<<(std::ostream&s,const UInt<value_size>&l){return OutputToStream<std::ostream,std::string>(s,l);}friend std::wostream&operator<<(std::wostream&s,const UInt<value_size>&l){return OutputToStream<std::wostream,std::wstring>(s,l);}private:template<class istream_type,class string_type,class char_type>static istream_type&InputFromStream(istream_type&s,UInt<value_size>&l){string_type ss;char_type z;s>>z;while(s.good()&&Misc::CharToDigit(z,10)>=0){ss+=z;z=static_cast<char_type>(s.get());}s.unget();l.FromString(ss);return s;}public:friend std::istream&operator>>(std::istream&s,UInt<value_size>&l){return InputFromStream<std::istream,std::string,char>(s,l);}friend std::wistream&operator>>(std::wistream&s,UInt<value_size>&l){return InputFromStream<std::wistream,std::wstring,wchar_t>(s,l);}private:uint Rcl2_one(uint c);uint Rcr2_one(uint c);uint Rcl2(uint bits,uint c);uint Rcr2(uint bits,uint c);public:static const char*LibTypeStr();static LibTypeCode LibType();uint Add(const UInt<value_size>&ss2,uint c=0);uint AddInt(uint value,uint index=0);uint AddTwoInts(uint x2,uint x1,uint index);static uint AddVector(const uint*ss1,const uint*ss2,uint ss1_size,uint ss2_size,uint*result);uint Sub(const UInt<value_size>&ss2,uint c=0);uint SubInt(uint value,uint index=0);static uint SubVector(const uint*ss1,const uint*ss2,uint ss1_size,uint ss2_size,uint*result);static sint FindLeadingBitInWord(uint x);static sint FindLowestBitInWord(uint x);static uint SetBitInWord(uint&value,uint bit);static void MulTwoWords(uint a,uint b,uint*result_high,uint*result_low);static void DivTwoWords(uint a,uint b,uint c,uint*r,uint*rest);};template<>class UInt<0>{public:uint table[1];void Mul2Big(const UInt<0>&,UInt<0>&){};void SetZero(){};uint AddTwoInts(uint,uint,uint){return 0;};};}namespace ttmath{template<uint value_size>const char*UInt<value_size>::LibTypeStr(){static const char info[]="asm_gcc_32";return info;}template<uint value_size>LibTypeCode UInt<value_size>::LibType(){LibTypeCode info=asm_gcc_32;return info;}template<uint value_size>uint UInt<value_size>::Add(const UInt<value_size>&ss2,uint c){uint b=value_size;uint*p1=table;uint*p2=const_cast<uint*>(ss2.table);uint dummy,dummy2;__asm__ __volatile__("xorl %%edx,%%edx \n""negl %%eax \n""1:\n""movl (%%esi,%%edx,4),%%eax \n""adcl %%eax,(%%ebx,%%edx,4)\n""incl %%edx \n""decl %%ecx \n""jnz 1b \n""adc %%ecx,%%ecx \n":"=c"(c),"=a"(dummy),"=d"(dummy2):"0"(b),"1"(c),"b"(p1),"S"(p2):"cc","memory");return c;}template<uint value_size>uint UInt<value_size>::AddInt(uint value,uint index){uint b=value_size;uint*p1=table;uint c;uint dummy,dummy2;__asm__ __volatile__("subl %%edx,%%ecx \n""1:\n""addl %%eax,(%%ebx,%%edx,4)\n""jnc 2f \n""movl $1,%%eax \n""incl %%edx \n""decl %%ecx \n""jnz 1b \n""2:\n""setc %%al \n""movzx %%al,%%edx \n":"=d"(c),"=a"(dummy),"=c"(dummy2):"0"(index),"1"(value),"2"(b),"b"(p1):"cc","memory");return c;}template<uint value_size>uint UInt<value_size>::AddTwoInts(uint x2,uint x1,uint index){uint b=value_size;uint*p1=table;uint c;uint dummy,dummy2;__asm__ __volatile__("subl %%edx,%%ecx \n""addl %%esi,(%%ebx,%%edx,4)\n""incl %%edx \n""decl %%ecx \n""1:\n""adcl %%eax,(%%ebx,%%edx,4)\n""jnc 2f \n""mov $0,%%eax \n""incl %%edx \n""decl %%ecx \n""jnz 1b \n""2:\n""setc %%al \n""movzx %%al,%%eax \n":"=a"(c),"=c"(dummy),"=d"(dummy2):"0"(x2),"1"(b),"2"(index),"b"(p1),"S"(x1):"cc","memory");return c;}template<uint value_size>uint UInt<value_size>::AddVector(const uint*ss1,const uint*ss2,uint ss1_size,uint ss2_size,uint*result){uint rest=ss1_size-ss2_size;uint c;uint dummy1,dummy2,dummy3;__asm__ __volatile__("push %%edx \n""xor %%edx,%%edx \n""1:\n""mov (%%esi,%%edx,4),%%eax \n""adc (%%ebx,%%edx,4),%%eax \n""mov %%eax,(%%edi,%%edx,4)\n""inc %%edx \n""dec %%ecx \n""jnz 1b \n""adc %%ecx,%%ecx \n""pop %%eax \n""or %%eax,%%eax \n""jz 3f \n""xor %%ebx,%%ebx \n""neg %%ecx \n""mov %%eax,%%ecx \n""2:\n""mov (%%esi,%%edx,4),%%eax \n""adc %%ebx,%%eax \n""mov %%eax,(%%edi,%%edx,4)\n""inc %%edx \n""dec %%ecx \n""jnz 2b \n""adc %%ecx,%%ecx \n""3:\n":"=a"(dummy1),"=b"(dummy2),"=c"(c),"=d"(dummy3):"1"(ss2),"2"(ss2_size),"3"(rest),"S"(ss1),"D"(result):"cc","memory");return c;}template<uint value_size>uint UInt<value_size>::Sub(const UInt<value_size>&ss2,uint c){uint b=value_size;uint*p1=table;uint*p2=const_cast<uint*>(ss2.table);uint dummy,dummy2;__asm__ __volatile__("xorl %%edx,%%edx \n""negl %%eax \n""1:\n""movl (%%esi,%%edx,4),%%eax \n""sbbl %%eax,(%%ebx,%%edx,4)\n""incl %%edx \n""decl %%ecx \n""jnz 1b \n""adc %%ecx,%%ecx \n":"=c"(c),"=a"(dummy),"=d"(dummy2):"0"(b),"1"(c),"b"(p1),"S"(p2):"cc","memory");return c;}template<uint value_size>uint UInt<value_size>::SubInt(uint value,uint index){uint b=value_size;uint*p1=table;uint c;uint dummy,dummy2;__asm__ __volatile__("subl %%edx,%%ecx \n""1:\n""subl %%eax,(%%ebx,%%edx,4)\n""jnc 2f \n""movl $1,%%eax \n""incl %%edx \n""decl %%ecx \n""jnz 1b \n""2:\n""setc %%al \n""movzx %%al,%%edx \n":"=d"(c),"=a"(dummy),"=c"(dummy2):"0"(index),"1"(value),"2"(b),"b"(p1):"cc","memory");return c;}template<uint value_size>uint UInt<value_size>::SubVector(const uint*ss1,const uint*ss2,uint ss1_size,uint ss2_size,uint*result){uint rest=ss1_size-ss2_size;uint c;uint dummy1,dummy2,dummy3;__asm__ __volatile__("push %%edx \n""xor %%edx,%%edx \n""1:\n""mov (%%esi,%%edx,4),%%eax \n""sbb (%%ebx,%%edx,4),%%eax \n""mov %%eax,(%%edi,%%edx,4)\n""inc %%edx \n""dec %%ecx \n""jnz 1b \n""adc %%ecx,%%ecx \n""pop %%eax \n""or %%eax,%%eax \n""jz 3f \n""xor %%ebx,%%ebx \n""neg %%ecx \n""mov %%eax,%%ecx \n""2:\n""mov (%%esi,%%edx,4),%%eax \n""sbb %%ebx,%%eax \n""mov %%eax,(%%edi,%%edx,4)\n""inc %%edx \n""dec %%ecx \n""jnz 2b \n""adc %%ecx,%%ecx \n""3:\n":"=a"(dummy1),"=b"(dummy2),"=c"(c),"=d"(dummy3):"1"(ss2),"2"(ss2_size),"3"(rest),"S"(ss1),"D"(result):"cc","memory");return c;}template<uint value_size>uint UInt<value_size>::Rcl2_one(uint c){uint b=value_size;uint*p1=table;uint dummy,dummy2;__asm__ __volatile__("xorl %%edx,%%edx \n""negl %%eax \n""1:\n""rcll $1,(%%ebx,%%edx,4)\n""incl %%edx \n""decl %%ecx \n""jnz 1b \n""adcl %%ecx,%%ecx \n":"=c"(c),"=a"(dummy),"=d"(dummy2):"0"(b),"1"(c),"b"(p1):"cc","memory");return c;}template<uint value_size>uint UInt<value_size>::Rcr2_one(uint c){uint b=value_size;uint*p1=table;uint dummy;__asm__ __volatile__("negl %%eax \n""1:\n""rcrl $1,-4(%%ebx,%%ecx,4)\n""decl %%ecx \n""jnz 1b \n""adcl %%ecx,%%ecx \n":"=c"(c),"=a"(dummy):"0"(b),"1"(c),"b"(p1):"cc","memory");return c;}template<uint value_size>uint UInt<value_size>::Rcl2(uint bits,uint c){uint b=value_size;uint*p1=table;uint dummy,dummy2,dummy3;__asm__ __volatile__("push %%ebp \n""movl %%ecx,%%esi \n""movl $32,%%ecx \n""subl %%esi,%%ecx \n""movl $-1,%%edx \n""shrl %%cl,%%edx \n""movl %%edx,%%ebp \n""movl %%esi,%%ecx \n""xorl %%edx,%%edx \n""movl %%edx,%%esi \n""orl %%eax,%%eax \n""cmovnz %%ebp,%%esi \n""1:\n""roll %%cl,(%%ebx,%%edx,4)\n""movl (%%ebx,%%edx,4),%%eax \n""andl %%ebp,%%eax \n""xorl %%eax,(%%ebx,%%edx,4)\n""orl %%esi,(%%ebx,%%edx,4)\n""movl %%eax,%%esi \n""incl %%edx \n""decl %%edi \n""jnz 1b \n""and $1,%%eax \n""pop %%ebp \n":"=a"(c),"=D"(dummy),"=S"(dummy2),"=d"(dummy3):"0"(c),"1"(b),"b"(p1),"c"(bits):"cc","memory");return c;}template<uint value_size>uint UInt<value_size>::Rcr2(uint bits,uint c){uint b=value_size;uint*p1=table;uint dummy,dummy2,dummy3;__asm__ __volatile__("push %%ebp \n""movl %%ecx,%%esi \n""movl $32,%%ecx \n""subl %%esi,%%ecx \n""movl $-1,%%edx \n""shll %%cl,%%edx \n""movl %%edx,%%ebp \n""movl %%esi,%%ecx \n""xorl %%edx,%%edx \n""movl %%edx,%%esi \n""addl %%edi,%%edx \n""decl %%edx \n""orl %%eax,%%eax \n""cmovnz %%ebp,%%esi \n""1:\n""rorl %%cl,(%%ebx,%%edx,4)\n""movl (%%ebx,%%edx,4),%%eax \n""andl %%ebp,%%eax \n""xorl %%eax,(%%ebx,%%edx,4)\n""orl %%esi,(%%ebx,%%edx,4)\n""movl %%eax,%%esi \n""decl %%edx \n""decl %%edi \n""jnz 1b \n""roll $1,%%eax \n""andl $1,%%eax \n""pop %%ebp \n":"=a"(c),"=D"(dummy),"=S"(dummy2),"=d"(dummy3):"0"(c),"1"(b),"b"(p1),"c"(bits):"cc","memory");return c;}template<uint value_size>sint UInt<value_size>::FindLeadingBitInWord(uint x){sint result;uint dummy;__asm__ ("movl $-1,%1 \n""bsrl %2,%0 \n""cmovz %1,%0 \n":"=r"(result),"=&r"(dummy):"r"(x):"cc");return result;}template<uint value_size>sint UInt<value_size>::FindLowestBitInWord(uint x){sint result;uint dummy;__asm__ ("movl $-1,%1 \n""bsfl %2,%0 \n""cmovz %1,%0 \n":"=r"(result),"=&r"(dummy):"r"(x):"cc");return result;}template<uint value_size>uint UInt<value_size>::SetBitInWord(uint&value,uint bit){uint old_bit;uint v=value;__asm__ ("btsl %%ebx,%%eax \n""setc %%bl \n""movzx %%bl,%%ebx \n":"=a"(v),"=b"(old_bit):"0"(v),"1"(bit):"cc");value=v;return old_bit;}template<uint value_size>void UInt<value_size>::MulTwoWords(uint a,uint b,uint*result_high,uint*result_low){uint result1_;uint result2_;__asm__ ("mull %%edx \n":"=a"(result1_),"=d"(result2_):"0"(a),"1"(b):"cc");*result_low=result1_;*result_high=result2_;}template<uint value_size>void UInt<value_size>::DivTwoWords(uint a,uint b,uint c,uint*r,uint*rest){uint r_;uint rest_;__asm__ ("divl %%ecx \n":"=a"(r_),"=d"(rest_):"0"(b),"1"(a),"c"(c):"cc");*r=r_;*rest=rest_;}}
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <map>

const int bits = 4;

ttmath::UInt<bits> randUInt() {
	ttmath::UInt<bits> n;
	for (unsigned int i = 0; i < bits; i++) {
		srand(time(NULL));
		n.table[i] = rand();	
	}
	return n;
}

ttmath::UInt<bits> mod_exp(ttmath::UInt<bits> base, ttmath::UInt<bits> exponent, ttmath::UInt<bits> N) {
	if (N == 1) {
		return 0;
	}
	ttmath::UInt<bits> res = 1;
	base = base % N;
	while (exponent > 0) {
		if (exponent % 2 == 1) {
			res = (res*base) % N;
		}
		exponent = exponent >> 1;
		base = (base * base) % N;
	}
	return res;
}

ttmath::UInt<bits> gcd(ttmath::UInt<bits> a, ttmath::UInt<bits> b) {
	ttmath::UInt<bits> t;
	while(b != 0) {
		t = b;
		b = a % b;
		a = t;	
	}

	return a;
}

bool isPrimeNaive(ttmath::UInt<bits> n) {
	if(n < 2) return false;
	if(n == 2) return true;
   	if(n % 2 == 0) return false;
    	for(ttmath::UInt<bits> i=3; (i*i)<=n; i+=2){
        	if(n % i == 0 ) return false;
   	 }
    	return true;
}

bool isPrime(ttmath::UInt<bits> n, int k) {
	
	//return isPrimeNaive(n);

	if (n < 100) {
		return isPrimeNaive(n);
	}

	ttmath::UInt<bits> x, a, d, r, tmp;


	if(n % 2 == 0) {
		return false;
	}
	if(n < 7) {
		return true;
	}
	
	d = n-1;
	r = 0;
	while(d % 2 == 0) {
		d = d / 2;
		r++;
	}

	for(int i = 0; i < k; i++) {
		a = randUInt();
        	a = a % (n-4) + 2;
		x = mod_exp(a, d, n);	
		if (x == 1 || x == n-1) {
			continue;
		}
		for (ttmath::UInt<bits> j = 0; j < (r - 1); j++) {
			x = mod_exp(x, 2, n);
			if (x == 1) {
				return false;
			}
			if (x == n-1) {
				continue;
			}
		}
		return false;	
	}
	return true;
}

bool trivial(ttmath::UInt<bits> factor, ttmath::UInt<bits> N) {
	if(factor == 1 || factor == -1 ||factor==N||factor==N-N-N) {
		return true;
	}else{
		return false;	
	}
}

ttmath::UInt<bits> trial(ttmath::UInt<bits> N) {
	for(ttmath::UInt<bits> i = 2; i <= (N/2); i++) {
		if (N % i == 0) {
			return i;
		}
	}
	return -1;
}

ttmath::UInt<bits> g(ttmath::UInt<bits>	x, ttmath::UInt<bits> N) {
	return (x * x + 1) % N;
}

ttmath::UInt<bits> polles(ttmath::UInt<bits> N) {
	ttmath::UInt<bits> x = 2;
	ttmath::UInt<bits> y = 2;
	ttmath::UInt<bits> d = 1;
	ttmath::UInt<bits> tmp;

	if (isPrime(N, 5)) {
		//std::cout << N << " is a prime " << std::endl;
		return -1;
	}

	if (N < 1000) {
		//std::cout << N << " < 1000" << std::endl;
		return trial(N);
	}

	while(d == 1) {
		x = g(x, N);
		y = g(g(y, N), N);
		d = gcd(std::max(x, y) - std::min(x, y), N);
	}
	if (d == N) {
		if (! isPrime(d, 5)) {
			//std::cout << d << " == " << N << std::endl;
			return trial(N);
		}
		return -1;
	} else {
		//std::cout << "found d: " << d << std::endl;
		if (isPrimeNaive(d) == false) {
			//std::cout << d << " is not a factor!" << std::endl;
			return trial(d);
		}
		return d;
	}
}

ttmath::UInt<bits> pollardsRho(ttmath::UInt<bits> N) {

	ttmath::UInt<bits> x0, x1, tmp, y0, y1, b;

	if (isPrime(N, 10)) {
		return N;
	}
	
	for(unsigned int j = 0; j < bits; j++) {
		srand(time(NULL));
		x0.table[j] = rand();
	}

	x0 = x0 % N;
	y0 = x0;

	b = 1;

	while(true) {
		tmp = x0;
		tmp.Pow(2);
		x1 = (tmp + b) % N;
		tmp = y0;
		tmp.Pow(2);
		y1 = (tmp + b);
		y1.Pow(2);
		y1 = (y1 + b) % N;
		tmp = gcd(std::max(x1, y1) - std::min(x1, y1), N);

		if (tmp == N) {
			b++;
			continue;
		}	
	
		if(!trivial(tmp, N) && isPrime(tmp, 5)) {
			return tmp;
		}	

		x0 = x1;
		y0 = y1;
	}
}

int main() {

	ttmath::UInt<bits> N, factor;
	
	std::string s;
	std::vector<std::string> factors;

	std::map<ttmath::UInt<bits>, ttmath::UInt<bits>> map;

	while(true) {
		factor = 1;

		std::cin >> N;

		if(N == 0) {
			break;
		}	
		
		while (factor != -1) {
			factor = polles(N);
			//factor = pollardsRho(N);
			//std::cout << "factor for N = " << N << " -> " << factor << std::endl;
			if (factor == -1) {
				//std::cout << "map["<<N<<"] is cur "<< map[N] << std::endl;
				map[N]++;
				//std::cout << "map["<<N<<"] is now "<< map[N] << std::endl;
				//std::cout << N << " should be added to map" << std::endl;
				break;
			}

			//std::cout << "map["<<factor<<"] is cur "<< map[factor] << std::endl;
			map[factor]++;
			//std::cout << "map["<<factor<<"] is now "<< map[factor] << std::endl;
			N = N / factor;
			//std::cout << "new N = " << N << std::endl;
		}		
		
		int test = 1;
		for(auto const &it : map) {
			s = s + it.first.ToString() + "^" + it.second.ToString();
			map[it.first] = 0;
			if (test < map.size()) {
				s = s + " ";
			}
			test++;
		}
		factors.push_back(s);
		s = "";
		
		map.clear();
	}
	
	for(int i = 0; i < factors.size(); i++) {
		std::cout << factors[i] << std::endl;
	}
	

	return 0;
}
