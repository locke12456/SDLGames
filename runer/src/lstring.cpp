#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "UsrFuc.h"
#include "lstring.h"
/*		Public		*/
LString::LString()
{
	init();
}
LString::LString(const char *Str)
{
	int i;
	init();
	add(Str);
	lenth=&LEN;
	string=(const char *)(String);
}
bool LString::operator ==(const LString& b)
{
	int i;
	for(i=0;i<STRING_LEN;i++){
		if(*(string+i)!=*(b.string+i))
			break;
	}
	return (*(string+i)==*(b.string+i));
}
bool LString::operator ==(const char *Str)
{
	int i;
	for(i=0;i<LEN;i++){
		if((*(string+i)!=*(Str+i)))
			break;
	}
	return (*(string+i)==*(Str+i));
}
LString& LString::operator +=(const LString& b)
{
	add(b.string,*(b.lenth));
	return *(this);
}

LString LString::operator +(const LString& b)
{
	LString a=LString(this->string);
	a+=b;
	return a;
}
LString& LString::operator =(const char *Str)
{
	LString a=LString(Str);
	*(this)=a;
	return *(this);
}

LString& LString::operator +=(const char *Str)
{
	LString a=LString(Str);
	*(this)+=a;
	return *(this);
}

LString LString::operator +(const char *Str)
{
	LString a=LString(Str);
	LString b=LString(this->string);
	b+=a;
	return b;
}


/*		Private		*/
void LString::init()
{
	memset(this,0,sizeof(LString));
}
void LString::add(const char *Str)
{
	static int length = 0;
	if(LEN>=STRING_LEN||*(Str+(length))==0){
		length = 0;
		return;
	}
	*(String+(LEN++))=*(Str+length);
	if(*(Str+(++length))!=0)
		add(Str);
	else{
		length = 0;
		return;
	}
}
void LString::add(const char *Str,int len)
{
	static int length = 0;
	if(length>=len||LEN>=STRING_LEN)
	{
		length = 0;
		return;
	}
	*(String+(LEN++))=*(Str+length);
	if(*(Str+(++length))!=0)
		add(Str,len);
	else
	{
		length = 0;
		return;
	}
}
/*		Out Side	*/
bool operator ==(const LString& a,const LString& b)
{
	int i;
	for(i=0;i<STRING_LEN;i++){
		if((*(a.string+i)!=*(b.string+i)))
			break;
	}
	return (*(a.string+i)==*(b.string+i));
}