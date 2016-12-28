typedef struct
{
	int x,y;
}POINT;
class ELF
{
public:
	ELF(int page);
	POINT * getElfs(int site);
	void combine();
	void Release();
private:
	SDL_Surface * temp;
	POINT * xy;
	int Len;
};