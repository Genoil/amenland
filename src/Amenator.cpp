#include <vector>
#include <chrono>
#include "plugin.hpp"
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#define SAMPLE_BPM 138

struct Sample {
	std::string path;
	vector<float> buffer;
	float position = 0;
	unsigned int channels;
	unsigned int sampleRate;
	drwav_uint64 sampleCount;

	void load(std::string path) {
		DEBUG("loading sample: %s", path.c_str());
		buffer.resize(0);

		unsigned int c;
		unsigned int sr;
		drwav_uint64 sc;
		float *pSampleData;

		std::string assetPath = asset::plugin(pluginInstance, path);
		pSampleData = drwav_open_file_and_read_pcm_frames_f32(assetPath.c_str(), &c, &sr, &sc, NULL);
		DEBUG("sample data info: channels=%d, rate=%d, pcmcount=%d", c, sr, (int)sc);

		if (pSampleData != NULL) {
			channels = c;
			sampleRate = sr;
			buffer.clear();
			for (unsigned int i=0; i < sc*c; i++) {
				buffer.push_back(pSampleData[i]);		
			}
			sampleCount = buffer.size()/c;
			DEBUG("sample data info: totalSampleCount=%d", (int)sampleCount);
			drwav_free(pSampleData, NULL);
			DEBUG("successfully loaded sample: %s", path.c_str());
		}
		else {
			DEBUG("failed to load sample: %s", path.c_str());
		}		
	}
};

struct Amenator : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		BPM_INPUT,
		GATE_INPUT,
		SLICE_INPUT,
		ENV_INPUT,
		INPUTS_LEN
	};
	
	enum OutputId {
		LEFT_OUTPUT,
		RIGHT_OUTPUT,
		ZWIJN_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		LIGHTS_LEN
	};

	bool loading = false;
	bool run = false;
	float framesPerSlice;
	bool playZwijn = false;
	unsigned int zwijnCounter = 0;

	struct Sample amen;
	struct Sample zwijn;

	dsp::SchmittTrigger playTrigger;
	dsp::SchmittTrigger bpmTrigger;

	int64_t lastFrame = -1;
	float speed = 1;

	Amenator() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(BPM_INPUT, "");
		configInput(GATE_INPUT, "");
		configInput(SLICE_INPUT, "");
		configInput(ENV_INPUT, "");
		configInput(ZWIJN_OUTPUT, "");
		configOutput(LEFT_OUTPUT, "");
		configOutput(RIGHT_OUTPUT, "");

		loading = true;

		amen.load("res/samples/amen.wav");
		zwijn.load("res/samples/zwijn.wav");

		framesPerSlice = amen.sampleRate * 30.f/(float)SAMPLE_BPM;
		DEBUG("frames per slice: %f", framesPerSlice);

		loading = false;
	}

	void process(const ProcessArgs& args) override {
		if(loading) return;

		if (inputs[BPM_INPUT].isConnected()) {
			if(bpmTrigger.process(inputs[BPM_INPUT].getVoltage())) {
				if(lastFrame >=0) {
					float bpm = 60.f/(float(args.frame - lastFrame)/args.sampleRate);
					speed = amen.sampleRate/args.sampleRate * bpm/SAMPLE_BPM;
				}
				lastFrame = args.frame;
				zwijnCounter++;
				if(zwijnCounter % 16 == 0) {
					playZwijn = true;
				}
			}
		}

		if (inputs[GATE_INPUT].isConnected()) {
			if (playTrigger.process(inputs[GATE_INPUT].getVoltage())) {
				run = true;
				if (inputs[SLICE_INPUT].isConnected()) {
					float voltage = inputs[SLICE_INPUT].getVoltage();
					float key = floor(12 * voltage );
					if(key < 0) key = 0;
					else if(key > 23) key = 23;
					DEBUG("voltage: %.3f, key: %.3f", voltage, key);
					amen.position = key * framesPerSlice; 
				}
				else {
					amen.position = 0;
				}
			}
		}

		float env;
		if (inputs[ENV_INPUT].isConnected()) {
			env = inputs[ENV_INPUT].getVoltage();
		}
		else {
			env = 5.0;
		}

		if (run && floor(amen.position) < amen.sampleCount) { 

			int index = 2*floor(amen.position);
			float left  = env * amen.buffer[index];
			float right = env * amen.buffer[index+1];

			if (outputs[LEFT_OUTPUT].isConnected() && !outputs[RIGHT_OUTPUT].isConnected()) {
				outputs[LEFT_OUTPUT].setVoltage((left+right)/2);
			}		
			else if (outputs[LEFT_OUTPUT].isConnected() && outputs[RIGHT_OUTPUT].isConnected()) {
				outputs[LEFT_OUTPUT].setVoltage(left);
				outputs[RIGHT_OUTPUT].setVoltage(right);
			}
			amen.position += speed;
			
		}
		else { 
			run = false;
			outputs[LEFT_OUTPUT].setVoltage(0);
			outputs[RIGHT_OUTPUT].setVoltage(0);
		}

		if(outputs[ZWIJN_OUTPUT].isConnected() && playZwijn) {
			if(floor(zwijn.position) < zwijn.sampleCount) {
				outputs[ZWIJN_OUTPUT].setVoltage(zwijn.buffer[zwijn.position]);
				zwijn.position += zwijn.sampleRate/args.sampleRate;
			}
			else {
				zwijn.position = 0;
				zwijnCounter = 0;
				playZwijn = false;
			}
		}
	}
	
};

struct ZwijnPort : app::SvgPort {
	ZwijnPort() {
		setSvg(Svg::load(asset::plugin(pluginInstance,"res/components/ZwijnPort.svg")));
	}
};

struct AmenatorWidget : ModuleWidget {
	AmenatorWidget(Amenator* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Amenator.svg")));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 22.715)), module, Amenator::BPM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.475, 37.81)), module, Amenator::GATE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 53.195)), module, Amenator::SLICE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.475, 68.29)), module, Amenator::ENV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 86.07)), module, Amenator::LEFT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 101.6)), module, Amenator::RIGHT_OUTPUT));
		addOutput(createOutputCentered<ZwijnPort>(mm2px(Vec(7.62, 118.595)), module, Amenator::ZWIJN_OUTPUT));

	}
};


Model* modelAmenator = createModel<Amenator, AmenatorWidget>("Amenator");