namespace MadLibrary {

enum KeyType : char {
    A_UPPER = 65,  // A
    B_UPPER,       // B
    C_UPPER,       // C
    D_UPPER,       // D
    E_UPPER,       // E
    F_UPPER,       // F
    G_UPPER,       // G
    H_UPPER,       // H
    I_UPPER,       // I
    J_UPPER,       // J
    K_UPPER,       // K
    L_UPPER,       // L
    M_UPPER,       // M
    N_UPPER,       // N
    O_UPPER,       // O
    P_UPPER,       // P
    Q_UPPER,       // Q
    R_UPPER,       // R
    S_UPPER,       // S
    T_UPPER,       // T
    U_UPPER,       // U
    V_UPPER,       // V
    W_UPPER,       // W
    X_UPPER,       // X
    Y_UPPER,       // Y
    Z_UPPER,       // Z

    A_LOWER = 97,  // a
    B_LOWER,       // b
    C_LOWER,       // c
    D_LOWER,       // d
    E_LOWER,       // e
    F_LOWER,       // f
    G_LOWER,       // g
    H_LOWER,       // h
    I_LOWER,       // i
    J_LOWER,       // j
    K_LOWER,       // k
    L_LOWER,       // l
    M_LOWER,       // m
    N_LOWER,       // n
    O_LOWER,       // o
    P_LOWER,       // p
    Q_LOWER,       // q
    R_LOWER,       // r
    S_LOWER,       // s
    T_LOWER,       // t
    U_LOWER,       // u
    V_LOWER,       // v
    W_LOWER,       // w
    X_LOWER,       // x
    Y_LOWER,       // y
    Z_LOWER,       // z
};
}  // namespace MadLibrary

namespace Private {
char get_character() {
    char buf = 0;
    if (read(0, &buf, 1) < 0) perror("read()");
    return (buf);
}

MadLibrary::KeyType get_key_type() {
    char ch = Private::get_character();

    return MadLibrary::KeyType{ch};
}

}  // namespace Private
