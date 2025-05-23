/**
   LV2 headers are based on the URI of the specification they come from, so a
   consistent convention can be used even for unofficial extensions.  The URI
   of the core LV2 specification is <http://lv2plug.in/ns/lv2core>, by
   replacing `http:/` with `lv2` any header in the specification bundle can be
   included, in this case `lv2.h`.
*/
#include <lv2/core/lv2.h>

/** Include standard C headers */
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum { AMP_GAIN = 0, AMP_INPUT = 1, AMP_OUTPUT = 2 } PortIndex;

typedef struct {
  // Port buffers
  const float *gain;
  const float *input;
  float *output;
} Amp;

static LV2_Handle instantiate(const LV2_Descriptor *descriptor, double rate,
                              const char *bundle_path,
                              const LV2_Feature *const *features) {
  Amp *amp = (Amp *)calloc(1, sizeof(Amp));

  return (LV2_Handle)amp;
}

static void connect_port(LV2_Handle instance, uint32_t port, void *data) {
  Amp *amp = (Amp *)instance;

  switch ((PortIndex)port) {
  case AMP_GAIN:
    amp->gain = (const float *)data;
    break;
  case AMP_INPUT:
    amp->input = (const float *)data;
    break;
  case AMP_OUTPUT:
    amp->output = (float *)data;
    break;
  }
}

static void activate(LV2_Handle instance) {}

/** Define a macro for converting a gain in dB to a coefficient. */
#define DB_CO(g) ((g) > -90.0f ? powf(10.0f, (g) * 0.05f) : 0.0f)

static void run(LV2_Handle instance, uint32_t n_samples) {
  const Amp *amp = (const Amp *)instance;

  const float gain = *(amp->gain);
  const float *const input = amp->input;
  float *const output = amp->output;

  const float coef = DB_CO(gain);

  for (uint32_t pos = 0; pos < n_samples; pos++) {
    output[pos] = input[pos] * coef;
  }
}

static void deactivate(LV2_Handle instance) {}

static void cleanup(LV2_Handle instance) { free(instance); }

static const void *extension_data(const char *uri) { return NULL; }

static const LV2_Descriptor descriptor = {AMP_URI,  instantiate,   connect_port,
                                          activate, run,           deactivate,
                                          cleanup,  extension_data};

LV2_SYMBOL_EXPORT const LV2_Descriptor *lv2_descriptor(uint32_t index) {
  return index == 0 ? &descriptor : NULL;
}
