#include "config.h"

typedef struct {
    int start, end;
} Match_Indices;

typedef struct {
    void *ptr;
} Pattern;

typedef enum {
    MATCH_SUCCEEDED, MATCH_FAILED, MATCH_ABORTED
} Match_Result;

extern Pattern new_pattern(const char *pattern, int case_matters);
extern Match_Result match_pattern(Pattern p, const char *string,
				Match_Indices * indices, int is_reverse);
extern void free_pattern(Pattern p);
