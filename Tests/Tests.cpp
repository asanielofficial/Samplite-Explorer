#include <cstdio>
#include <cstring>
#include "../Source/Library.h"

static int failures = 0;
static void check (bool ok, const char* what)
{
    std::printf ("%s  %s\n", ok ? "PASS" : "FAIL", what);
    if (! ok) ++failures;
}

int main()
{
    using namespace analysis;
    check (bpmFromName ("drum_loop_128bpm") == 128, "bpm: suffix");
    check (bpmFromName ("Break 92 BPM") == 92, "bpm: separate word");
    check (bpmFromName ("kick_01") == 0, "bpm: index numbers ignored");
    check (keyFromName ("pad_Am") == 21, "key: Am");
    check (keyFromName ("bass_F#min_120") == 18, "key: F#min");
    check (keyFromName ("chord Bbmaj") == 10, "key: Bbmaj");
    check (keyFromName ("Dog_bark") == -1, "key: ordinary words ignored");

    const double sr = 44100;
    for (double bpm : { 90.0, 120.0, 140.0 })
    {
        std::vector<float> x ((size_t) (sr * 8), 0.0f);
        juce::Random random (1);
        const double step = sr * 60.0 / bpm;
        for (double start = 0; start < (double) x.size(); start += step)
            for (int i = 0; i < 2000 && (size_t) start + (size_t) i < x.size(); ++i)
                x[(size_t) start + (size_t) i] = (random.nextFloat() * 2 - 1) * std::exp (-i / 300.0f);
        const double got = detectBpm (x.data(), (int) x.size(), sr);
        std::printf ("      click track %.0f -> %.0f\n", bpm, got);
        check (std::abs (got - bpm) <= 2, "bpm: detected from click track");
    }

    auto chord = [sr] (std::initializer_list<double> notes)
    {
        std::vector<float> x ((size_t) (sr * 3), 0.0f);
        for (double f : notes)
            for (size_t i = 0; i < x.size(); ++i)
                x[i] += 0.2f * (float) std::sin (juce::MathConstants<double>::twoPi * f * (double) i / sr);
        return x;
    };
    auto c = chord ({ 261.63, 329.63, 392.00 });
    auto am = chord ({ 220.00, 261.63, 329.63 });
    auto tone = chord ({ 440.0 });
    check (detectKey (c.data(), (int) c.size(), sr) == 0, "key: C major triad");
    check (detectKey (am.data(), (int) am.size(), sr) == 21, "key: A minor triad");
    check (detectKey (tone.data(), (int) tone.size(), sr) == -1, "key: single tone left blank");

    check (detectBpm (c.data(), (int) c.size(), sr) == 0, "bpm: sustained chord left blank");
    check (detectBpm (am.data(), (int) am.size(), sr) == 0, "bpm: sustained minor chord left blank");

    check (tuningFromName ("pad_432Hz") == 432, "tuning: from name");
    check (tuningFromName ("pad 440 Hz") == 440, "tuning: separate unit");
    check (tuningFromName ("sub_100hz") == 0, "tuning: other frequencies ignored");
    for (double a4 : { 432.0, 440.0, 446.0 })
    {
        const double r = a4 / 440.0;
        auto t = chord ({ 220.00 * r, 261.63 * r, 329.63 * r });
        const double got = detectTuning (t.data(), (int) t.size(), sr);
        std::printf ("      chord at %.0f -> %.2f\n", a4, got);
        check (std::abs (got - a4) < 1.0, "tuning: detected from chord");
        auto one = chord ({ a4 });
        check (std::abs (detectTuning (one.data(), (int) (sr * 0.3), sr) - a4) < 1.0, "tuning: short single note");
    }
    std::vector<float> noise ((size_t) (sr * 3));
    juce::Random rng (7);
    for (auto& v : noise) v = rng.nextFloat() - 0.5f;
    check (detectTuning (noise.data(), (int) noise.size(), sr) == 0, "tuning: noise left blank");

    auto dir = juce::File::createTempFile ("presets");
    dir.createDirectory();
    auto vital = dir.getChildFile ("a.vital");
    vital.replaceWithText (R"({"preset_name":"Warm Pad","author":"me","preset_style":"Pad"})");
    check (presets::read (vital).name == "Warm Pad", "preset: Vital");

    char fxp[60] = {};
    std::memcpy (fxp, "CcnK", 4);
    std::memcpy (fxp + 16, "XfsX", 4);
    std::memcpy (fxp + 28, "Big Lead", 8);
    auto serum = dir.getChildFile ("b.fxp");
    serum.replaceWithData (fxp, sizeof (fxp));
    check (presets::read (serum).name == "Big Lead" && presets::read (serum).synth == "Serum", "preset: Serum fxp");

    auto serum2 = dir.getChildFile ("c.SerumPreset");
    serum2.replaceWithText (R"(XferJson..{"presetName":"Glass {Keys}","presetAuthor":"me"}..binary)");
    check (presets::read (serum2).name == "Glass {Keys}", "preset: SerumPreset (assumed layout)");

    auto broken = dir.getChildFile ("d.fxp");
    broken.replaceWithText ("junk");
    check (presets::read (broken).name == "d", "preset: damaged file falls back to filename");
    dir.deleteRecursively();

    std::printf ("%d failure(s)\n", failures);
    return failures == 0 ? 0 : 1;
}
