#ifndef STD_CTRS_H
#define STD_CTRS_H

#ifdef __cplusplus
/** no idea why would anyone use this lib when std::vector, std::list,
    std::map, etc. are readily available, but anyway let`s be prepared **/
#define typeof decltype
extern "C" {
#endif

#if !defined(CTR_MGET)
    #include <stdlib.h>
    #define CTR_MGET realloc
#endif

#define _CTR_B(type) /**  base  **/ struct { const unsigned size; type _; }
#define  CTR_V(type) /** vector **/ struct { _CTR_B(type * const); }
#define  CTR_L(type) /**  list  **/ struct { _CTR_B(type); const void *l, *r; }

#define CTR_V_MGET(v, s) do {                                        \
    *(void**)&(v)._ = CTR_MGET((s) ? (v)._ : 0, s * sizeof(*(v)._)); \
    if ((*(unsigned*)&(v).size = s) == 0)                            \
        *(void**)&(v)._ = 0;                                         \
} while (0)

#define CTR_V_SORT(v, cmp) /** heapsort **/                                   \
for (unsigned _n, _c, _s = (v).size, _p = _s >> 1; _s > 1;                    \
    (_p > 1) ? _p-- : _s--) {                                                 \
    typeof((v)._) _d = (v)._ - 1;                                             \
    typeof(*(v)._) _t;                                                        \
    for (_c = 0, _n = _p; _n <= _s >> 1; _c++)                                \
        _n = 2 * _n + ((2 * _n < _s) && (cmp(_d + 2 * _n + 1, _d + 2 * _n))); \
    for (; (_n != _p) && (cmp(_d + _p, _d + _n)); _c--, _n >>= 1);            \
    for (_t = _d[_p]; _c > 0; _c--, _d[_n >> (_c + 1)] = _d[_n >> _c]);       \
    _d[_n] = _t;                                                              \
    if (_p == 1) {                                                            \
        _t = _d[_p];                                                          \
        _d[_p] = _d[_s];                                                      \
        _d[_s] = _t;                                                          \
    }                                                                         \
}



#ifdef __cplusplus
}
#undef typeof
#endif
#endif /** STD_CTRS_H **/
