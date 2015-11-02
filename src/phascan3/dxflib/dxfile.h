#ifndef __DXFILE_H__
#define __DXFILE_H__

#include "dxfbase.h"

typedef struct _Dxfile Dxfile;
struct _Dxfile {
    GMappedFile *mmpedFile;
    gchar *contents;
    gchar *cur;
    gint len;
};

extern Dxfile *dxfile_open(const gchar *filename);
extern void dxfile_destory(Dxfile *f);

static inline gboolean dxfile_is_end(Dxfile *f) { return *f->cur == '\0'; }

static inline void dxfile_goto_nextline(Dxfile *f)
{
    g_return_if_fail( f != NULL );
    g_return_if_fail( f->cur != NULL );
    for (; *f->cur && *f->cur != '\n'; ++f->cur ) {}
    if (*f->cur == '\n') { ++f->cur; }
}

extern gboolean dxfile_set_pos(Dxfile *f, glong pos);
static inline glong dxfile_get_pos(Dxfile *f) { return (glong)(f->cur - f->contents); }

static inline void dxfile_get_int(Dxfile *f, gint *i) { sscanf(f->cur, "%d", i); dxfile_goto_nextline(f);}
static inline void dxfile_get_int16(Dxfile *f, gint16 *i) { sscanf(f->cur, "%hd", i); dxfile_goto_nextline(f); }
static inline void dxfile_get_double(Dxfile *f, gdouble *d) { sscanf(f->cur, "%lf", d); dxfile_goto_nextline(f);}

extern void dxfile_get_line(Dxfile *f, gchar **str, gsize *len);

static inline void dxfile_get_code(Dxfile *f, gint *i) { *i=DXF_CODE_INVALID; dxfile_get_int(f, i); }
extern void dxfile_get_section(Dxfile *f, Section *s);


/*选择code模板*/
#define SWITCH_CODE_BEGIN(f)   \
{\
    gint code = 0; \
    glong pos = dxfile_get_pos(f); \
    for ( dxfile_get_code(f, &code); \
          code && (code != DXF_CODE_INVALID); ) { \
            switch(code) {
#define SWITCH_CODE_DO(cond)  break; case (cond):
#define SWITCH_CODE_DO_FOLLOW(cond) case (cond):
#define SWITCH_CODE_OUT()  code=0; continue;
#define SWITCH_CODE_OUT_OFFSET(f) pos = dxfile_get_pos(f); SWITCH_CODE_OUT()
#define SWITCH_CODE_SEEK(f)  dxfile_set_pos(f, pos)
#define SWITCH_CODE_END(f)  break;\
        default: \
            c_debug("[%s] invalid code %d", __func__, code); \
            dxfile_goto_nextline(f);\
            break; \
        } \
        pos = dxfile_get_pos(f); \
        dxfile_get_code(f, &code); \
    } \
    if ( code == DXF_CODE_INVALID ) { c_debug("[%s] Can't get Code", __func__); }\
    dxfile_set_pos(f, pos);\
}

#endif /* __DXFILE_H__ */
