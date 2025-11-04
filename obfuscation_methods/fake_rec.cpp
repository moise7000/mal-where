#include <iostream>

/**
 * @brief Wrapper de fonction qui stocke le type de retour.
 * Permet de connaître le type de retour sans avoir à le spécifier.
 * 
 * @tparam R Type de retour de la fonction
 * @tparam Arg Type de l'argument de la fonction
 */
template <typename R, typename Arg>
struct FunctionWrapper {
    typedef R result_type;
    typedef Arg argument_type;
    
    R (*func)(Arg);
    
    FunctionWrapper(R (*f)(Arg)) : func(f) {}
    
    R operator()(Arg arg) const {
        return func(arg);
    }
};

/**
 * @brief Fonction helper pour créer un wrapper facilement.
 */
template <typename R, typename Arg>
FunctionWrapper<R, Arg> make_wrapper(R (*f)(Arg)) {
    return FunctionWrapper<R, Arg>(f);
}

/**
 * @brief Fonction récursive factice qui exécute f(arg) x fois.
 * Les x-1 premières fois calculent mais ignorent le résultat.
 * À la x-ème exécution, retourne le résultat de f(arg).
 * 
 * Compatible GCC 4.4.7, sans pointeur exposé à l'utilisateur.
 * 
 * @tparam F Type du wrapper de fonction
 * @param x Nombre d'itérations
 * @param f Le wrapper de fonction à exécuter
 * @param arg L'argument à passer à f
 * @return Le résultat de f(arg) à la dernière itération
 */
template <typename F>
typename F::result_type fake_recursive(int x, F f, typename F::argument_type arg) {
    if (x <= 0) {
        return typename F::result_type();
    }
    
    if (x == 1) {
        return f(arg);
    } else {
        f(arg); 
        return fake_recursive(x - 1, f, arg);
    }
}
