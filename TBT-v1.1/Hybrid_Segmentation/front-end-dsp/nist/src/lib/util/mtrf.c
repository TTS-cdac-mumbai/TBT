/* File: mtrf.c */
/* converted from spmalloc to be a general memory tracing function */

#include <util/utillib.h>

static int mtrf_dealloc = 1;
static int mtrf_verbose = 0;

char *mtrf_malloc(int bytes)
{
    char *cp;
    if (bytes < 0)
	return((char *)0);
    if (bytes == 0)
	bytes = 1;
    cp=malloc(bytes);
    if (mtrf_verbose) {
#ifdef HAVE_PTR_FMT
	fprintf(stderr,"Malloc %p %d\n",cp,bytes), fflush(stderr);
#else
	fprintf(stderr,"Malloc %x %d\n",(unsigned)cp,bytes), fflush(stderr);
#endif
    }
    return cp;
}

char *mtrf_realloc(char *ptr, int bytes)
{
    char *cp;

    cp = realloc(ptr, bytes);
    if (cp != ptr)
	if (mtrf_verbose) {
#ifdef HAVE_PTR_FMT
	    fprintf(stderr,"REALLOC\nFree %p\nMalloc %p %d\n",ptr,
		    cp,bytes), fflush(stderr);
#else
	    fprintf(stderr,"REALLOC\nFree %x\nMalloc %x %d\n",(unsigned)ptr,
		    (unsigned)cp,bytes), fflush(stderr);
#endif
	}
    return cp;
}

int mtrf_free(char *p)
{
    int rtn;

    rtn = 1;
    if (mtrf_dealloc)
	free(p);
    if (mtrf_verbose) {
#ifdef HAVE_PTR_FMT
	fprintf(stderr,"Free %p\n",p), fflush(stderr);
#else
	fprintf(stderr,"Free %x\n",(unsigned)p), fflush(stderr);
#endif
    }
    return(rtn);
}

char *mtrf_strdup(char *p)
{
    char *cp;
    int len;
    len = ((strlen(p) == 0) ? 1 : strlen(p)) + 1;

    cp=mtrf_malloc(len);
    strcpy(cp,p);
    if (mtrf_dealloc == 0) {
#ifdef HAVE_PTR_FMT
	fprintf(stderr,"Malloc %p %d\n",cp,len), fflush(stderr);
#else
	fprintf(stderr,"Malloc %x %d\n",(unsigned)cp,len), fflush(stderr);
#endif
    }
    return(cp);
}

void mtrf_set_dealloc(int n)
{
    mtrf_dealloc = n;
}

void mtrf_set_verbose(int n)
{
    mtrf_verbose = n;
}

int mtrf_get_dealloc(void)
{
    return mtrf_dealloc;
}

int mtrf_get_verbose(void)
{
    return mtrf_verbose;
}

