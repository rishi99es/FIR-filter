/**
 * Script to generate filter coefficients for a linear phase, FIR bandpass filter 
 * centered at 5000 Hz.
 *
 * author: Rishi K Shukla
 * website: www.technogeek310.wordpress.com
 *
*/

/* n: determines the number of filter coefficients to be generated. */
n = 63;

/**
 * bnd_edge: determines filter's frequency window. In Scilab, the frequency 
 *        window can vary from 0 to 0.5 only. Here, 
 * f1 = 4420/44100 = 0.1 (lower cut-off frequency)
 * f2 = 5580/44100 = 0.13 (higher cut-off frequency)
 * f0 = (f1 + f2) / 2 = (0.1 + 0.13) / 2
 * f0 = 0.115
 * Central frequency = fo * sampling frequency = 0.115 * 44100
 * Central frequency = 5071.5 Hz(approx. 5000 Hz)
*/
bnd_edge = [0, 4410/44100; 4420/44100, 5580/44100; 5590/44100, 22050/44100];

/* This is the desired magnitude for the central frequencies of the spectrum. */
des_magnit = [0 1 0];

/* This defines magnitude of weighted error across the frequency spectrum. */
rel_wght_err = [1 1 1];

/* generate band-pass filter coefficients as floating point values. */
[hn] = eqfir(n, bnd_edge, des_magnit, rel_wght_err);

/* save the coefficients in a text file */
print('coeff-44100.txt', hn);

/* convert floating point coefficients to signed int16_t format. */
hn = hn * 32768;
hn = round(hn);

/**
 * hm - amplitude of the output
 * fr - frequency 
 * 256 indicates the length of the array that will be returned 
*/
[hm, fr] = frmag(hn, 256);

/* plot relationship between frequency and amplitude. */
plot(fr, hm);

xlabel('Normalized Digital Frequency');
ylabel('Magnitude');
title('Frequency Response of FIR BPF using REMEZ algorithm');
