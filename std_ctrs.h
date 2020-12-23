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
#define _CTR_ASSIGN0(d, s) do {                    \
    auto __d = &(d);                               \
    decltype(d) __s = {};                          \
    _Pragma("pack(push, 1)")                       \
    struct S {                                     \
        unsigned char _[sizeof(d)];                \
    } *_d = (struct S*)__d, *_s = (struct S*)&__s; \
    _Pragma("pack(pop)")                           \
    *_d = *_s;                                     \
} while (0)
#define _CTR_ASSIGN1(d, s) do {                    \
    auto __d = &(d);                               \
    _Pragma("pack(push, 1)")                       \
    struct S {                                     \
        unsigned char _[sizeof(d)];                \
    } *_d = (struct S*)__d, *_s = (struct S*)&(s); \
    _Pragma("pack(pop)")                           \
    *_d = *_s;                                     \
} while (0)

/** get capacity of the vector **/
#define  CTR_V_CGET(v) _CTR_V_CGET(&(v))
unsigned _CTR_V_CGET(void *v) {
    _CTR_B(unsigned) *_v;
    *(void**)&_v = v;
    return (_v->mult) ? ((_v->size + _v->mult - 1) / _v->mult) * _v->mult : 0;
}

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

/** heapsort **/
#define CTR_V_SORT(v, comp) do {                                            \
    auto _v = &(v);                                                         \
    decltype(_v->_) _d = _v->_ - 1;                                         \
    decltype(*_v->_) _t;                                                    \
    for (unsigned _n, _c, _s = _v->size, _p = _s >> 1; _s > 1;              \
        (_p > 1) ? _p-- : _s--) {                                           \
        for (_c = 0, _n = _p; _n <= _s >> 1; _c++)                          \
            _n = 2 * _n + ((2 * _n < _s) &&                                 \
                           (comp(_d + 2 * _n + 1, _d + 2 * _n) > 0));       \
        for (; (_n != _p) && (comp(_d + _p, _d + _n) > 0); _c--, _n >>= 1); \
        for (_t = _d[_p]; _c > 0; _c--, _d[_n >> (_c + 1)] = _d[_n >> _c]); \
        _d[_n] = _t;                                                        \
        if (_p == 1) {                                                      \
            _t = _d[_p];                                                    \
            _d[_p] = _d[_s];                                                \
            _d[_s] = _t;                                                    \
        }                                                                   \
    }                                                                       \
} while (0)



#ifdef __cplusplus
}
#endif
#endif /** STD_CTRS_H **/
