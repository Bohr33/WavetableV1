#FFT/Wavetable/Nyquist Limit Math

The purpose of this write up is to outline the math required for determining what harmonics you want to get rid of when filtering our harmonics/bins when using an FFT. This was written in conjunction with a wavetable project where I am documenting the steps and challenges along the way. This deals specifically with the math behind managing your FFT Data.



##Outline
In my current example, I am using an FFT operation to create mipmaps for a wavetable generator. The user can upload an arbitrary wavetable to this mipmap generator and it will perform an FFT and remove unecessary frequencies for each mip map. 

###Question
Our question is, for each mipmap (i.e. for each octave of frequencies) which bins will I have to filter?

###JUCE's FFT
When performing an FFT on a data set of size $N$, the output of the table will be of size $N * 2$, where each sample from the original table produces a pair of real and imaginary values for each, and the implied negative spectrum is also encoded, but unused on our end. When performing operations using JUCE's FFT, you will need to use a buffer of size $N * 2$

 These values encode the amplitude and phase of the harmonic at a specific frequency. These frequency bins are laid out as harmonics of the fundamental, thus, the first bin will correspond to the fundamental frequency, while the second bin is 2* the fundamental frequency, etc. This means that for an octave of notes C3 - B3, the highest our highest fundamental note will be B3, $f = 247$ multiplied by the sampling rate divided by this fundamental frequency.
 
 $$
 \large
	\text{max frequency} =  \text{floor}(SR/f) \cdot f
 $$
 
 For our context however, we will be generating these mipmaps from a loop from each octave, 