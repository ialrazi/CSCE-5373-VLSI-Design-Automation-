#ifndef DEFS_H
#define DEFS_H


#define SOURCE_ID 		-1
#define TARGET_ID 		-2
#define GRAPH_TIC		15
#define INFO_INTERVAL 	10		// seconds
#define MAX_FILEN		200
#define VCG_TYPE 		true
#define HCG_TYPE 		false
#define ROTATED			true
#define NOTROTATED		false
#define HOUR			3600		// seconds per hour
#define MINUTE			60			// seconds per minute
#define SNAP_LOC		"../outputs/FP"

#define MOVE_CNT		3			// Number of moves available
#define NAME_LEN		20
typedef enum {swap_ps, swap_ns, rotate} moveType_t;
const char moveNames[][NAME_LEN] = {"swap_ps", "swap_ns", "rotate"};


typedef bool graphType_t;


#define PLT_HEADER		"# ignore the lines starting with #\n# First number is the number of records\n# second number is the Time of the best record, zero based index\n\n"
#define COL_NAMES       "#index  Temperature         Area         FLP file              Aspect ratio    Density    Time (sec)\n#-----  ------------       ------       -----------------     --------------  ---------  -----------\n"


#endif //DEFS_H


