(int x, int y, int limit, double inv_w, double inv_h, int* r) {
    V zero, one, four, Civ, Crv, rv = { 0.0 };

    void writeOutput() {
        for (int i = 0; i < NV; ++i) {
            r[i] = rv[i] ? rv[i] + 1 : 0;
        }
    }

    void initVectors(double civ) {
        for (int i = 0; i < NV; ++i) {
            Crv[i] = (x + i) * inv_w - 1.5;
            Civ[i] = civ;
            zero[i] = 0.0;
            one[i] = 1.0;
            four[i] = 4.0;
        }
    }

    initVectors(y * inv_h - 1.0);

    V Zrv = zero, Ziv = zero, Trv = zero, Tiv = zero;

    for (V i = zero; i[0] < limit && IS(EQ(rv, zero)); i += one) {
        Ziv = Zrv * Ziv + Zrv * Ziv + Civ;
        Zrv = Trv - Tiv + Crv;
        Trv = Zrv * Zrv;
        Tiv = Ziv * Ziv;

        V escaped = GT(Trv + Tiv, four);
        V shouldSet = AND(EQ(rv, zero), escaped);
        rv = OR(rv, AND(shouldSet, i));
    }

    writeOutput();
}
