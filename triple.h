#ifndef TRIPLE_H
#define TRIPLE_H

/* A simple container for three values,
 * working as a value
 */
template<class T1, class T2, class T3>
struct triple
{
    T1 fst;
    T2 snd;
    T3 trd;

    triple() {}
    triple(T1 fst, T2 snd, T3 trd) : fst(fst), snd(snd), trd(trd) {}
    triple(const triple &t) : fst(t.fst), snd(t.snd), trd(t.trd) {}
};

// Common triple types
typedef triple<double, double, double> dtriple;

#endif // TRIPLE_H
