#include "../../include/Simulated/SimulatedSignal.h"


void add_awgn_noise_complex(std::vector<std::complex<float>>& signal, float target_snr_db) {
    int N = signal.size();

    // ??????
    double signal_power = 0.0;
    for (const auto& s : signal) {
        signal_power += std::norm(s);  // |s|^2
    }
    signal_power /= N;

    // ??????
    double snr_linear = pow(10.0, target_snr_db / 10.0);
    double noise_power = signal_power / snr_linear;
    double noise_std = sqrt(noise_power / 2);  // ???????????

    // ???????
    std::default_random_engine generator(std::random_device{}());
    std::normal_distribution<double> distribution(0.0, noise_std);

    // ????
    for (auto& s : signal) {
        s += std::complex<double>(distribution(generator), distribution(generator));
    }
}

// Generate one LFM pulse as complex<float>
std::vector<std::complex<float>> generate_lfm_pulse() {
    int pulse_samples = static_cast<int>(PULSE_WIDTH * SAMPLE_RATE);
    double K = BANDWIDTH / PULSE_WIDTH;  // chirp rate (Hz/s)
    std::vector<std::complex<float>> pulse(PRI_SAMPLES);

    for (int n = 0; n < PRI_SAMPLES; ++n) {
        if (n < pulse_samples) {
          double t = n / SAMPLE_RATE;
          double phase = 2.0 * PI * (0.5 * K * t * t);
          pulse[n] = std::complex<float>(std::cos(phase), std::sin(phase));
        }
        else {
          pulse[n] = std::complex<float>(0.0, 0.0);
        }
    }
    
    std::cout << pulse.size() << std::endl;
    
    return pulse;
}

// Zero-pad to full PRI length and replicate for 4 channels
std::vector<cshort> generate_multichannel_lfm() {
    auto pulse = generate_lfm_pulse();
    // add_awgn_noise_complex(pulse, 10.0);
    std::vector<cshort> channels(NUM_CHANNELS * PRI_SAMPLES);

    for (int n = 0; n < PRI_SAMPLES * NUM_CHANNELS; n+=NUM_CHANNELS) {
        std::complex<float> val =  + (n / NUM_CHANNELS < pulse.size()) ? pulse[n / NUM_CHANNELS] : std::complex<float>(0.0f, 0.0f);
        channels[n] = cshort(static_cast<short>(val.real() * SCALE),
                                     static_cast<short>(val.imag() * SCALE));
        channels[n+1] = cshort(static_cast<short>(val.real() * SCALE),
                                     static_cast<short>(val.imag() * SCALE));
        channels[n+2] = cshort(static_cast<short>(val.real() * SCALE),
                                     static_cast<short>(val.imag() * SCALE));
        channels[n+3] = cshort(static_cast<short>(val.real() * SCALE),
                                     static_cast<short>(val.imag() * SCALE));
    }

    return channels;
}


Eigen::ArrayXXcf generate_cos_signal() {
    const float PI = 3.14159265358979323846f;
    const float c = 3e8f; 
    const float fc = 77e9f;         
    const float fs = 245.76e6f;      
    const int N = 2048;             
    const int M = 128;  
    const float B = 20e6f;  
    const float T_chirp = N / fs; 
    const float slope = B / T_chirp;

    const float R_target = 50.0f; 
    const float v_target = 20.0f;   
    const float tau = 2 * R_target / c; 
    const float fd = 2 * v_target * fc / c; 

    ArrayXf t = ArrayXf::LinSpaced(N, 0, (N - 1) / fs);

    ArrayXXcf beat_signals(M, N);

    for (int m = 0; m < M; ++m) {
        float t_m = m * T_chirp;

        //  s_tx(t) = exp(j*2*pi*(fc*t + 0.5*slope*t^2))
        ArrayXcf tx_chirp = (2.0f * PI * (fc * t + 0.5f * slope * t.square()));

        //  s_rx(t) = exp(j*2*pi*(fc*(t - tau) + 0.5*slope*(t - tau)^2 + fd * t_m))
        ArrayXf t_delay = t - tau;
        ArrayXcf rx_chirp = (2.0f * PI * (fc * t_delay + 0.5f * slope * t_delay.square() + fd * t_m));

        // 
        ArrayXcf beat = rx_chirp * tx_chirp.conjugate();

        //
        beat_signals.row(m) = beat;
    }

    return beat_signals;
}