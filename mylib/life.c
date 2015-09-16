
#ifdef GLUKE

#   include <rgn.h>
#   include <utils.h>

#   define ALLOC rgn_alloc
#   define FREE rgn_free
#   define MEMSET(arr,c,nof_bytes) kmemset(arr,c,nof_bytes)

#else

#   include <PalmOS.h>

#   define ALLOC MemPtrNew
#   define FREE MemPtrFree
#   define MEMSET(arr,c,nof_bytes) MemSet(arr,nof_bytes,c)

#endif

#include <life.h>

static void pent_r(char *arena,int size_w,int size_h,int x,int y)
{
	arena[(y-1)*size_w+x]=1;
	arena[(y-1)*size_w+x+1]=1;
	arena[y*size_w+(x-1)]=1;
	arena[y*size_w+x]=1;
	arena[(y+1)*size_w+x]=1;
}		

static void boat(char *arena,int size_w,int size_h,int x,int y)
{
	arena[y*size_w+x-1]=1;
	arena[y*size_w+x]=1;
	arena[(y+1)*size_w+x-1]=1;
	arena[(y+1)*size_w+x+1]=1;
	arena[(y+2)*size_w+x]=1;
}

static void glider(char *arena,int size_w,int size_h,int x,int y)
{
	arena[y*size_w+x+1]=1;
	arena[(y+1)*size_w+x+2]=1;
	arena[(y+2)*size_w+x]=1;
	arena[(y+2)*size_w+x+1]=1;
	arena[(y+2)*size_w+x+2]=1;
}

static void tumbler(char *arena,int size_w,int size_h,int x,int y)
{
	arena[y*size_w+x+1]=1; 
	arena[y*size_w+x+2]=1; 
	arena[y*size_w+x+4]=1; 
	arena[y*size_w+x+5]=1; 
	arena[(y+1)*size_w+x+1]=1; 
	arena[(y+1)*size_w+x+2]=1; 
	arena[(y+1)*size_w+x+4]=1; 
	arena[(y+1)*size_w+x+5]=1; 
	arena[(y+2)*size_w+x+2]=1; 
	arena[(y+2)*size_w+x+4]=1; 
	arena[(y+3)*size_w+x]=1; 
	arena[(y+3)*size_w+x+2]=1; 
	arena[(y+3)*size_w+x+4]=1; 
	arena[(y+3)*size_w+x+6]=1; 
	arena[(y+4)*size_w+x]=1; 
	arena[(y+4)*size_w+x+2]=1; 
	arena[(y+4)*size_w+x+4]=1; 
	arena[(y+4)*size_w+x+6]=1; 
	arena[(y+5)*size_w+x]=1; 
	arena[(y+5)*size_w+x+1]=1; 
	arena[(y+5)*size_w+x+5]=1; 
	arena[(y+5)*size_w+x+6]=1;
}

static void line_osc(char *arena,int size_w,int size_h,int x,int y)
{
	arena[y*size_w+x]=1; 
	arena[y*size_w+x+3]=1; 
	arena[y*size_w+x+5]=1; 
	arena[y*size_w+x+6]=1; 
	arena[y*size_w+x+8]=1; 
	arena[y*size_w+x+11]=1; 
	arena[(y+1)*size_w+x]=1; 
	arena[(y+1)*size_w+x+1]=1; 
	arena[(y+1)*size_w+x+2]=1; 
	arena[(y+1)*size_w+x+3]=1; 
	arena[(y+1)*size_w+x+5]=1; 
	arena[(y+1)*size_w+x+6]=1; 
	arena[(y+1)*size_w+x+8]=1; 
	arena[(y+1)*size_w+x+9]=1; 
	arena[(y+1)*size_w+x+10]=1; 
	arena[(y+1)*size_w+x+11]=1; 
	arena[(y+2)*size_w+x]=1; 
	arena[(y+2)*size_w+x+3]=1; 
	arena[(y+2)*size_w+x+5]=1; 
	arena[(y+2)*size_w+x+6]=1; 
	arena[(y+2)*size_w+x+8]=1; 
	arena[(y+2)*size_w+x+11]=1;
}

static void bee_queen(char *arena,int size_w,int size_h,int x,int y,int osc)
{
	arena[y*size_w+x+3]=1;
	arena[y*size_w+x+5]=1;
	arena[(y+1)*size_w+x+1]=1;
	arena[(y+1)*size_w+x+5]=1;
	arena[(y+2)*size_w+x+1]=1;	
	arena[(y+3)*size_w+x]=1;	
	arena[(y+3)*size_w+x+5]=1;	
	arena[(y+4)*size_w+x+1]=1;		
	arena[(y+5)*size_w+x+1]=1;		
	arena[(y+5)*size_w+x+5]=1;		
	arena[(y+6)*size_w+x+3]=1;		
	arena[(y+6)*size_w+x+5]=1;
	
	if(osc)
	{
		arena[(y+3)*size_w+(x-6)]=1;
		arena[(y+3)*size_w+(x-6+1)]=1;
		arena[(y+4)*size_w+(x-6)]=1;
		arena[(y+4)*size_w+(x-6+1)]=1;		

		arena[(y+3)*size_w+(x+14)]=1;
		arena[(y+3)*size_w+(x+14+1)]=1;
		arena[(y+4)*size_w+(x+14)]=1;
		arena[(y+4)*size_w+(x+14+1)]=1;			
	}
}

static void st_life(int size_w,int size_h,
	struct life_object *life_objects,
	life_callback callback,void *user_data)
{
	char *arena,*count_arena;
	int w_mul_h,k,q,rq,x,rx,y,line_counter;
	
	w_mul_h=size_w*size_h;

	arena=ALLOC(w_mul_h);
	count_arena=ALLOC(w_mul_h);
		
	MEMSET(arena,0,w_mul_h);
	MEMSET(count_arena,0,w_mul_h);

	for(k=0;life_objects[k].type!=LIFE_OBJECT_TYPE_UNKNOWN;k++)
	{		
		switch(life_objects[k].type){
		case LIFE_OBJECT_TYPE_BOAT:
			boat(arena,size_w,size_h,life_objects[k].x,life_objects[k].y);
			break;
		case LIFE_OBJECT_TYPE_PENT_R:
			pent_r(arena,size_w,size_h,life_objects[k].x,life_objects[k].y);
			break;
		case LIFE_OBJECT_TYPE_TUMBLER:
			tumbler(arena,size_w,size_h,life_objects[k].x,life_objects[k].y);
			break;
		case LIFE_OBJECT_TYPE_LINE_OSC:
			line_osc(arena,size_w,size_h,life_objects[k].x,life_objects[k].y);
			break;
		case LIFE_OBJECT_TYPE_GLIDER:
			glider(arena,size_w,size_h,life_objects[k].x,life_objects[k].y);
			break;
		case LIFE_OBJECT_TYPE_QUEEN_BEE:
			bee_queen(arena,size_w,size_h,life_objects[k].x,life_objects[k].y,0);			
			break;
		case LIFE_OBJECT_TYPE_QUEEN_BEE_OSC:
			bee_queen(arena,size_w,size_h,life_objects[k].x,life_objects[k].y,1);
			break;			
		}
	}

	while(callback(arena,size_w,size_h,user_data)==0)
	{
		for(k=0,line_counter=0;k<w_mul_h;k++)
		{
			if(arena[k])
			{
				q=k-size_w;
				for(y=0;y<3;y++)
				{
					if(q<0) rq=w_mul_h+q;
					else if(q>=w_mul_h) rq=q-w_mul_h;
					else rq=q;
				
					for(x=-1;x<2;x++)
					{
						if(!(x==0 && y==1))
						{
							if(line_counter==0 && x==-1) rx=(size_w-1);
							else if(line_counter==size_w-1 && x==1) rx=-(size_w-1);
							else rx=x;
							count_arena[rq+rx]++;
						}
					}
					q+=size_w;
				}
			}
			if(++line_counter==size_w) line_counter=0;
		}
		for(k=0;k<w_mul_h;k++)
		{
			if(count_arena[k]<2) arena[k]=0;
			else if(count_arena[k]>3) arena[k]=0;
			else if(count_arena[k]==3) arena[k]=1;
			
			count_arena[k]=0;
		}
	}
	FREE(arena);
	FREE(count_arena);
}

struct life_object LIFE_SAMPLE_R_PENTOMINO[]={
	{.type=LIFE_OBJECT_TYPE_SAMPLE_R_PENTOMINO,.x=0,.y=0},
	{.type=LIFE_OBJECT_TYPE_UNKNOWN}		
};

struct life_object LIFE_SAMPLE_GLIDERS[]={
	{.type=LIFE_OBJECT_TYPE_SAMPLE_GLIDERS,.x=0,.y=0},	
	{.type=LIFE_OBJECT_TYPE_UNKNOWN}		
};

struct life_object LIFE_SAMPLE_OSCILLATORS[]={
	{.type=LIFE_OBJECT_TYPE_SAMPLE_OSCILLATORS},	
	{.type=LIFE_OBJECT_TYPE_UNKNOWN}
};

struct life_object LIFE_SAMPLE_QUEEN_BEE[]={
	{.type=LIFE_OBJECT_TYPE_SAMPLE_QUEEN_BEE},
	{.type=LIFE_OBJECT_TYPE_UNKNOWN}
};

void life(int size_w,int size_h,
	struct life_object *life_objects,
	life_callback callback,void *user_data)
{
	switch(life_objects[0].type){
	case LIFE_OBJECT_TYPE_SAMPLE_R_PENTOMINO:
		{
			struct life_object life_objects_loc[]={
				{.type=LIFE_OBJECT_TYPE_PENT_R,.x=size_w/2-1,.y=size_h/2-1},
				{.type=LIFE_OBJECT_TYPE_UNKNOWN}
			};

			st_life(size_w,size_h,life_objects_loc,callback,user_data);
		}
		break;
	case LIFE_OBJECT_TYPE_SAMPLE_GLIDERS:
		{
			struct life_object life_objects_loc[]={
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=0,.y=10},
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=5,.y=10},
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=10,.y=10},
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=15,.y=10},		
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=20,.y=10},				
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=25,.y=10},
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=30,.y=10},		
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=35,.y=10},
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=40,.y=10},		
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=45,.y=10},				
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=50,.y=10},
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=55,.y=10},
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=60,.y=10},		
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=65,.y=10},		
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=70,.y=10},		
				{.type=LIFE_OBJECT_TYPE_GLIDER,.x=75,.y=10},		
				{.type=LIFE_OBJECT_TYPE_UNKNOWN}		
			};

			st_life(size_w,size_h,life_objects_loc,callback,user_data);
		}
		break;
	case LIFE_OBJECT_TYPE_SAMPLE_OSCILLATORS:
		{
			struct life_object life_objects_loc[]={
				{.type=LIFE_OBJECT_TYPE_QUEEN_BEE_OSC,.x=12,.y=size_h/2-2},
				{.type=LIFE_OBJECT_TYPE_LINE_OSC,.x=40,.y=size_h/2},
				{.type=LIFE_OBJECT_TYPE_TUMBLER,.x=65,.y=size_h/2-2},
				{.type=LIFE_OBJECT_TYPE_UNKNOWN}
			};
			
			st_life(size_w,size_h,life_objects_loc,callback,user_data);
		}
		break;
	case LIFE_OBJECT_TYPE_SAMPLE_QUEEN_BEE:
		{
			struct life_object life_objects_loc[]={
				{.type=LIFE_OBJECT_TYPE_QUEEN_BEE,.x=size_w/2,.y=size_h/2-2},
				{.type=LIFE_OBJECT_TYPE_UNKNOWN}
			};

			st_life(size_w,size_h,life_objects_loc,callback,user_data);
		}
		break;
	default:
		st_life(size_w,size_h,life_objects,callback,user_data);
	}
}

