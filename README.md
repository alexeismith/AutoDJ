# AutoDJ

AutoDJ is a fully-automatic DJ system that uses audio analysis to construct a digital understanding of your music library.

The software was designed by Alexei Smith as part of a study exploring audio analysis and machine creativity through an automatic DJ system for house music:

#### Music Information Retrieval and AI: Automating Music Performance

<em>"Music Information Retrieval (MIR) allows musical knowledge to be extracted directly from audio data. The field of research is maturing and there are now many powerful techniques in temporal, tonal and higher-level analysis, which can be used in combination to build intelligent music systems. A number of automatic DJ systems have been developed in the past, but the primary focus has been on modelling a human DJ using MIR algorithms. This project looks at automatic DJ software from a user perspective, investigating how it can be designed to meet user needs, as well as where it fits into the larger world of DJing and music performance. AutoDJ is a fully-automatic desktop app, written in C++, which can produce a coherent mix of dance music tracks. The mixing decisions are driven by a combination of beat tracking, key signature detection, track segmentation and danceability analysis. Powerful off-beat transients are a stylistic feature of house music and surrounding dance-focused genres, which often confuse the phase estimation of beat-tracking algorithms. A novel method for correcting beat phase estimations is discussed, which uses correlation with pulse trains (Percival & Tzanetakis, 2014) to establish whether an existing estimate is offset by half a beat period. A pipeline for modelling the creativity of a human performer is proposed, with a discussion of how this can be integrated into AutoDJ. In a test with 18 users, the software was rated highly and results suggested there is a strong use case for automatic DJ software on desktop. Future open-source development objectives are proposed, with the aim of readying AutoDJ for a wider test and subsequent release. The testing methodology used here can serve as a framework for further investigations into the automation of music performance."</em>


## Installing

Please note that _Mac_ is currently the only supported platform.

If you just want to run the AutoDJ app, please see the [releases](https://github.com/alexeismith/AutoDJ/releases) for the latest build. There is no install process - just launch AutoDJ and select a folder of music!


### Building in your IDE

##### Install JUCE
1. Download and install [JUCE](https://juce.com/get-juce/download) (including the Projucer)
2. Set up [global module paths](https://forum.juce.com/t/global-module-paths/25299/3) in the Projucer

##### Clone this repo (in Terminal)
1. cd \<desired location\>
2. git clone https://github.com/alexeismith/AutoDJ.git
3. cd AutoDJ
3. git submodule update --init --recursive

##### Generate IDE Project
1. Open `AutoDJ.jucer`
2. Select your preferred IDE at the top - e.g. "Xcode (macOS)"
2. Click the Save and Open IDE button
3. Build and run using your IDE

## Contributing

Thanks for your interest in contributing to AutoDJ! Here's how to get involved...

1. Ideally, have a good understanding of C++ audio programming (and perhaps JUCE).
3. Read the [crash course](https://github.com/alexeismith/AutoDJ/wiki/AutoDJ-Crash-Course) on AutoDJ's architecture.
4. Tackle an open issue!

Issues suitable for first-time contributors are listed [here](https://github.com/alexeismith/AutoDJ/contribute), while _all_ issues can be found [here](https://github.com/alexeismith/AutoDJ/issues).

Please see the [wiki](https://github.com/alexeismith/AutoDJ/wiki) for more information.
