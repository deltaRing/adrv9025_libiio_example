#include "../../include/Settings/TestFFTTime.h"

void test_fft() {
    using cpx = complex<double>;
    const int N = 2048;
    const int num_threads = 4; 

    // ======  ======
    VectorXcd x(N);
    for (int n = 0; n < N; ++n)
        x(n) = cpx(cos(2 * M_PI * n / N), sin(2 * M_PI * n / N));

    // =============================
    // 
    // =============================
    MatrixXcd F(N, N);
    for (int k = 0; k < N; ++k)
        for (int n = 0; n < N; ++n)
            F(k, n) = polar(1.0, -2.0 * M_PI * k * n / N);

    auto t1 = high_resolution_clock::now();
    VectorXcd X_dft = F * x;
    auto t2 = high_resolution_clock::now();
    double time_dft = duration<double, milli>(t2 - t1).count();

    // =============================
    // 
    // =============================
    Eigen::FFT<double> fft;
    vector<cpx> x_vec(x.data(), x.data() + x.size());
    vector<cpx> X_eigen;

    auto t3 = high_resolution_clock::now();
    fft.fwd(X_eigen, x_vec);
    auto t4 = high_resolution_clock::now();
    double time_eigen = duration<double, milli>(t4 - t3).count();

    // =============================
    // 
    // =============================
    vector<cpx> X_fftw_st(N);
    fftw_plan plan_st = fftw_plan_dft_1d(N,
        reinterpret_cast<fftw_complex*>(x_vec.data()),
        reinterpret_cast<fftw_complex*>(X_fftw_st.data()),
        FFTW_FORWARD, FFTW_ESTIMATE);

    auto t5 = high_resolution_clock::now();
    fftw_execute(plan_st);
    auto t6 = high_resolution_clock::now();
    double time_fftw_st = duration<double, milli>(t6 - t5).count();
    fftw_destroy_plan(plan_st);

    // =============================
    // 
    // =============================
    cout << "=== FFT Time Compare (N = " << N << ") ===\n";
    cout << "1. DFT Multiply      : " << time_dft     << " ms\n";
    cout << "2. Eigen::FFT        : " << time_eigen   << " ms\n";
    cout << "3. FFTW Single Thread       : " << time_fftw_st << " ms\n";

    // =============================
    // 
    // =============================
    cout << "\n=== Compares ==" << endl;
    for (int i = 0; i < 3; ++i) {
        cout << "Index " << i << ":\n";
        cout << "  DFT     = " << X_dft[i]     << "\n";
        cout << "  Eigen   = " << X_eigen[i]   << "\n";
        cout << "  FFTW ST = " << X_fftw_st[i] << "\n";
    }
}