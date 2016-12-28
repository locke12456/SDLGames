#define	BUFFER_AMOUNT 2
#define	BUFFER_WIDTH SCREEN_WIDTH/2
#define	BUFFER_HEIGHT SCREEN_HEIGHT
#define LAYER_AMOUNT 10
#define	MAP_WIDTH 1920
#define	MAP_HEIGHT 8000
#define MAP_Y	80
enum MAP_BLOCK
{
	BLOCK_00,BLOCK_01,BLOCK_02,
	BLOCK_10,BLOCK_11,BLOCK_12,
	BLOCK_20,BLOCK_21,BLOCK_22,
	BLOCK_30,BLOCK_31,BLOCK_32,
	BLOCK_40,BLOCK_41,BLOCK_42,
	BLOCK_50,BLOCK_51,BLOCK_52,
	BLOCK_60,BLOCK_61,BLOCK_62,
	BLOCK_70,BLOCK_71,BLOCK_72,
	BLOCK_80,BLOCK_81,BLOCK_82,
	BLOCK_90,BLOCK_91,BLOCK_92
};
/*
Map bloack example:
	map's height = 1920 . width = 1920
	0 ======== 960 ======= 1280 ===== 1920   Y↓
	||			||			||			||	
	|| BLOCK_00	|| BLOCK_01 || BLOCK_02 ||	0~960
	||			||			||			||  
	960 ========||==========||==========||
	||			||			||			||
	|| BLOCK_10	|| BLOCK_11 || BLOCK_12 ||	480~1280
	||			||			||			||
	1280 =======||==========||==========||
	||			||			||			||	
	|| BLOCK_20	|| BLOCK_21 || BLOCK_22 ||	960~1920
	||			||			||			||
	1920 =================================
	X→	 0~960		480~1280	1280~1920		
*/
typedef struct MAP
{
	bool showFlag;
	SDL_Rect	*POSITION;
	SDL_Rect	MOTION_CENTER;
	int		TOP;
	int	WIDTH;
	int	HEIGHT;
	int	FRAME_WIDTH;
	int	FRAME_HEIGHT;
	unsigned int	AMOUNT;
	unsigned int	FRAME_INDEX;
}MAP_STRUCT;
typedef struct LAYER_STATUS
{
	bool	showFlag;
	int		Layer_num;
	void	(*draw)();
}LAYER_STRUCT;
class Multi_Map
{
public:
	Multi_Map();
	//~Multi_Map();
	Multi_Map(unsigned int amount,int Map_Index,int Map_Height,int Map_Width);//amount : Cell數量，Map_Index : Cell起始號碼 , Map_Height&Map_Width : 地圖總寬度與高度
	void Depth_Map(unsigned int amount,int Map_Index,int Map_Height,int Map_Width);//底圖  內容同上
	bool add_Draw(unsigned int layer,int layer_num,void (*draw)());//在地圖上增加一層圖層 ，layer : 增加至第幾個圖層 ，layer_num : 圖層編號，影響顯示在buffer的上下位置  draw : 自定義貼圖函式
	bool setMap_Center(int Which,SDL_Rect *xy);//Which : 設定哪張地圖， xy : 給定座標
	void Draw_Map(MAP *Maps); // 
	void Draw();//繪製地圖及圖層
private:
	OBJ draw_Map;
	void init();
	MAP		DepthMaps[BUFFER_AMOUNT];
	MAP		Maps[BUFFER_AMOUNT];
	//void	(*)();
	LAYER_STATUS	Layer[LAYER_AMOUNT];
//	void	
	
};