#ifndef STD_CTRS_H
#define STD_CTRS_H

#ifdef __cplusplus
/** no idea why would anyone want this lib when std::vector, std::list,
    std::map, etc. are readily available, but anyway let`s be prepared **/
extern "C" {
#else
#define decltype typeof
#define auto __auto_type
#endif

#if !defined(CTR_MGET)
    #include <stdlib.h>
    #define CTR_MGET realloc
#endif

/** container types **/
#define _CTR_B(type) /** base **/ struct { const unsigned size, mult; type _; }
#define  CTR_V(type) /** vect **/ struct { _CTR_B(type * const); }
#define  CTR_L(type) /** list **/ struct { _CTR_B(type); const void *l, *r; }

/** assignment (so that the compilation doesn`t fail on constant fields) **/
#define  CTR_ASSIGN(d, ...) _CTR_ASSIGN(d, ##__VA_ARGS__, 1, 0)
#define _CTR_ASSIGN(d, s, n, ...) _CTR_ASSIGN##n(d, s)
#define _CTR_ASSIGN0(d, s) do {                        \
    auto ___d = &(d);                                  \
    decltype(d) ___s = {};                             \
    _Pragma("pack(push, 1)")                           \
    struct S {                                         \
        unsigned char _[sizeof(d)];                    \
    } *__d = (struct S*)___d, *__s = (struct S*)&___s; \
    _Pragma("pack(pop)")                               \
    *__d = *__s;                                       \
} while (0)
#define _CTR_ASSIGN1(d, s) do {                        \
    auto ___d = &(d);                                  \
    _Pragma("pack(push, 1)")                           \
    struct S {                                         \
        unsigned char _[sizeof(d)];                    \
    } *__d = (struct S*)___d, *__s = (struct S*)&(s);  \
    _Pragma("pack(pop)")                               \
    *__d = *__s;                                       \
} while (0)

/** get capacity of the vector **/
#define CTR_V_CGET(_v) ({ auto v = &(_v); \
    (v->mult) ? ((v->size + v->mult - 1) / v->mult) * v->mult : 0; })

/** get or release memory for the vector **/
#define  CTR_V_MGET(v, ...) _CTR_V_MGET(v, ##__VA_ARGS__, 0, 0)
#define _CTR_V_MGET(v, s, m, ...) do {                                     \
    auto _v = &(v);                                                        \
    unsigned __m = (m), _m = !_v->mult || __m, _c = CTR_V_CGET(*_v);       \
    if (_m)                                                                \
        *(unsigned*)&_v->mult = __m ? __m : 1;                             \
    unsigned __s = (s), _s = ((__s + _v->mult - 1) / _v->mult) * _v->mult; \
    if (_m || (_s != _c))                                                  \
        *(void**)&_v->_ = CTR_MGET(_s ? _v->_ : 0, _s * sizeof(*_v->_));   \
    if (!__s)                                                              \
        *(void**)&_v->_ = 0;                                               \
    *(unsigned*)&_v->size = __s;                                           \
} while (0)

/** push_back equivalent **/
#define  CTR_V_PUSH(v, e, ...) _CTR_V_PUSH(v, e, ##__VA_ARGS__, 0)
#define _CTR_V_PUSH(v, e, m, ...) do {  \
    auto _v = &(v);                     \
    CTR_V_MGET(*_v, _v->size + 1, (m)); \
    _v->_[_v->size - 1] = (e);          \
} while (0)

/** filter and truncate the vector **/
#define CTR_V_FLTR(v, comp) do {                  \
    auto _v = &(v);                               \
    unsigned _d = 0;                              \
    for (unsigned _c = 0; _c < _v->size; _c++)    \
        if (comp((_v->_ + _c)) && (_d++ != _c))   \
            CTR_ASSIGN(_v->_[_d - 1], _v->_[_c]); \
    CTR_V_MGET(*_v, _d, 1);                       \
} while (0)

/** find a pattern in the vector **/
#define CTR_V_FIND(v, comp, b, e) ({                            \
    auto _v = &(v);                                             \
    unsigned _r = 0, _b = (b), _e = (e);                        \
    _b = ((_v->size < _b) && (_b != -1)) ? _v->size : _b;       \
    _e = ((_v->size < _e) && (_e != -1)) ? _v->size : _e;       \
    unsigned _p = (_b < _e) ? 1 : -1, _s = (_b > _e) ? _b : _e; \
    for (unsigned _c = (_b < _e) ? _b : _e; _c != _s; _c += _p) \
        if (comp((_v->_ + _c))) {                               \
            _r = _c + 1;                                        \
            break;                                              \
        }                                                       \
    _r;                                                         \
})

/** sort the vector using heapsort **/
#define CTR_V_SORT(v, comp, b, e) do {                                      \
    auto _v = &(v);                                                         \
    unsigned _b = (b), _e = (e);                                            \
    _b = (_v->size < _b) ? _v->size : _b;                                   \
    _e = (_v->size < _e) ? _v->size : _e;                                   \
    auto _d = _v->_ + ((_b < _e) ? _b : _e) - 1;                            \
    decltype(*_v->_) _t;                                                    \
    for (unsigned _n, _c, _s = (_b < _e) ? (_e - _b) : (_b - _e),           \
         _p = _s >> 1; _s > 1; (_p > 1) ? _p-- : _s--) {                    \
        for (_c = 0, _n = _p; _n <= _s >> 1; _c++)                          \
            _n = 2 * _n + ((2 * _n < _s) &&                                 \
                           (comp(_d + 2 * _n + 1, _d + 2 * _n) > 0));       \
        for (; (_n != _p) && (comp(_d + _p, _d + _n) > 0); _c--, _n >>= 1); \
        CTR_ASSIGN(_t, _d[_p]);                                             \
        for (; _c > 0; _c--)                                                \
            CTR_ASSIGN(_d[_n >> _c], _d[_n >> (_c - 1)]);                   \
        CTR_ASSIGN(_d[_n], _t);                                             \
        if (_p == 1) {                                                      \
            CTR_ASSIGN(_t, _d[_p]);                                         \
            CTR_ASSIGN(_d[_p], _d[_s]);                                     \
            CTR_ASSIGN(_d[_s], _t);                                         \
        }                                                                   \
    }                                                                       \
} while (0)



#ifdef __cplusplus
}
#endif
#endif /** STD_CTRS_H **/
