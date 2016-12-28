#define STRING_LEN 32

class LString
{
public:
	LString();
	LString(const char *Str);


	LString& operator=(const char *Str);

	bool operator ==(const LString& b);
	bool operator ==(const char *Str);

	LString operator+(const LString& b);
	LString operator+(const char *Str);

	LString& operator+=(const LString& b);
	LString& operator+=(const char *Str);

	//LString& operator-=(const LString& b);
	//LString& operator-=(const char *Str);

	//LString operator-(const LString& b);
	//LString operator-(const char *Str);

	const int *lenth;
	const char *string;
private:
	void init();
	void add(const char *Str);
	void add(const char *Str,int len);
	void sub(const char *Str);
	char String[STRING_LEN];
	int LEN;
};
bool operator ==(const LString& a,const LString& b);
