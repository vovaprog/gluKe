#ifndef LIFE_H_INCLUDED
#define LIFE_H_INCLUDED

#define LIFE_OBJECT_TYPE_UNKNOWN               0
#define LIFE_OBJECT_TYPE_BOAT                  1
#define LIFE_OBJECT_TYPE_PENT_R                2
#define LIFE_OBJECT_TYPE_LINE_OSC              3
#define LIFE_OBJECT_TYPE_TUMBLER               4
#define LIFE_OBJECT_TYPE_GLIDER                5
#define LIFE_OBJECT_TYPE_FISH                  6
#define LIFE_OBJECT_TYPE_QUEEN_BEE             7
#define LIFE_OBJECT_TYPE_QUEEN_BEE_OSC         8
#define LIFE_OBJECT_TYPE_SAMPLE_R_PENTOMINO    9
#define LIFE_OBJECT_TYPE_SAMPLE_GLIDERS       10
#define LIFE_OBJECT_TYPE_SAMPLE_OSCILLATORS   11
#define LIFE_OBJECT_TYPE_SAMPLE_QUEEN_BEE     12

struct life_object{
	int type;
	int x,y;
};

typedef int life_callback(char *arena,int size_w,int size_h,void *user_data);

extern struct life_object LIFE_SAMPLE_R_PENTOMINO[];
extern struct life_object LIFE_SAMPLE_GLIDERS[];
extern struct life_object LIFE_SAMPLE_OSCILLATORS[];
extern struct life_object LIFE_SAMPLE_QUEEN_BEE[];

void life(int size_w,int size_h,
	struct life_object *life_objects,
	life_callback callback,void *user_data);

#endif

