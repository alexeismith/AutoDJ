#pragma once

#include <JuceHeader.h>

class BeatUtils {
  public:
    struct ConstRegion {
        double firstBeat;
        double beatLength;
    };

    static double calculateBpm(const std::vector<double>& beats,
            int sampleRate);

    static std::vector<ConstRegion> retrieveConstRegions(
            const std::vector<double>& coarseBeats,
            int sampleRate);

    static double calculateAverageBpm(int numberOfBeats,
            int sampleRate,
            double lowerFrame,
            double upperFrame);

    static double makeConstBpm(
            const std::vector<ConstRegion>& constantRegions,
            int sampleRate,
            double* pFirstBeat);

    static double adjustPhase(
            double firstBeat,
            double bpm,
            int sampleRate,
            const std::vector<double>& beats);

    static std::vector<double> getBeats(const std::vector<ConstRegion>& constantRegions);

  private:
    static double roundBpmWithinRange(double minBpm, double centerBpm, double maxBpm);
};
