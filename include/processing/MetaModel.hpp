
/*
	InACTually
	2022

	participants:
	Lars Engeln - mail@lars-engeln.de

	interactive theater for actual acts
*/

#pragma once

#include "ProcNodeBase.hpp"

using namespace ci;
using namespace ci::app;


namespace act {
	namespace proc {

		struct MetaModel
		{
		public:
			static std::shared_ptr<MetaModel> create() { return std::make_shared<MetaModel>(); };

			//
			float arousal		= 0.0f; // 0..1 [low..high],			Angeregtheit / Erregung des Gesamtsystemes
			//-> arousal

			float speed			= 0.0f; // 0..1 [slow..fast],			Geschwindigkeit/Schnelligkeit
			float length		= 0.0f; // 0..1 [short..long],			length = duration, Dauer
			
			float dynamic		= 0.0f; // 0..1 [calm..dynamic],		Dynamik
			//-------> dynamic
			float linearity		= 0.0f; // 0..1 [chaotic..linear],		Linearität
			float periodicity	= 0.0f; // 0..1 [chaotic..evenly],		Periodizität
			float frequency		= 0.0f; // 0..1 [low..high],			Frequenz
			//<------- dynamic

			float structure		= 0.0f; // 0..1 [even..structured],		Struktur
			//----> structure
			float clarity		= 0.0f; // 0..1 [diffuse..clear],		Klarheit
			//-------> clarity
			float complexity	= 0.0f; // 0..1 [simple..complex],		Komplexität = 1 - Einfachheit
			float sharpness		= 0.0f; // 0..1 [dull..sharp],			Schärfe
			//<------- clarity
			float regularity	= 0.0f; // 0..1 [chaotic..regular],		An-/Ordnung
			float orientation	= 0.0f; // 0..1 [chaotic..aligned],		Orientation
			//float "Feinteiligkeit" // not used
			float granularity	= 0.0f; // 0..1 [gritty..fine],			Granularität
			float hierarchy		= 0.0f; // 0..1 [flat..nested],			Hierarchie // ist das nicht auch eher complexity?
			//<---- structure
			
			float dominance		= 0.0f; // 0..1 [submissive..dominant], Dominanz
			//----> dominance
			float size			= 0.0f; // 0..1 [small..big],			Größe
			float intensity		= 0.0f; // 0..1 [weak..strong],			Intensität
			//-------> intensity
			float temperature	= 0.0f; // 0..1 [cold..hot],			Temperatur
			float contrast		= 0.0f; // 0..1 [low..high],			Kontrast
			float saturation	= 0.0f; // 0..1 [low..high],			Sättigung
			//<------- intensity
			//<---- dominance
			//<- arousal

			void calc() {
				calcClarity(false);
				calcIntensity(true);
			}
			void calcArousal() {
				arousal = (speed + length + dynamic + structure + dominance) * 0.2f;
			}
			void calcDynamic(bool doCalcParent = true) {
				dynamic = (linearity + periodicity + frequency) * 0.333333f;
				if (doCalcParent)
					calcArousal();
			}
			void calcStructure(bool doCalcParent = true) {
				structure = (clarity + regularity + orientation + granularity + hierarchy) * 0.2f;
				if (doCalcParent)
					calcArousal();
			}
			void calcDominance(bool doCalcParent = true) {
				dominance = (size + intensity) * 0.5f;
				if (doCalcParent)
					calcArousal();
			}
			void calcClarity(bool doCalcParent = true) {
				clarity = (complexity + sharpness) * 0.5f;
				if (doCalcParent)
					calcStructure(doCalcParent);
			}
			void calcIntensity(bool doCalcParent = true) {
				intensity = (temperature + contrast + saturation) * 0.333333f;
				if(doCalcParent)
					calcDominance(doCalcParent);
			}

			ci::Json toJson() {
				ci::Json json = ci::Json::object();
				json["arousal"]		= arousal;
				json["speed"]		= speed;

				json["dynamic"]		= dynamic;
				json["linearity"]	= linearity;
				json["periodicity"]	= periodicity;
				json["frequency"]	= frequency;

				json["structure"]	= structure;
				json["clarity"]		= clarity;
				json["complexity"]	= complexity;
				json["sharpness"]	= sharpness;
				json["regularity"]	= regularity;
				json["orientation"]	= orientation;
				json["granularity"]	= granularity;
				json["hierarchy"]	= hierarchy;

				json["dominance"]	= dominance;
				json["size"]		= size;
				json["intensity"]	= intensity;
				json["temperature"]	= temperature;
				json["contrast"]	= contrast;
				json["saturation"]	= saturation;
				return json;
			};

			void fromJson(ci::Json json) {
				util::setValueFromJson(json, "arousal",		arousal);
				util::setValueFromJson(json, "speed",		speed);

				util::setValueFromJson(json, "dynamic",		dynamic);
				util::setValueFromJson(json, "linearity",	linearity);
				util::setValueFromJson(json, "periodicity", periodicity);
				util::setValueFromJson(json, "frequency",	frequency);

				util::setValueFromJson(json, "structure",	structure);
				util::setValueFromJson(json, "clarity",		clarity);
				util::setValueFromJson(json, "complexity",	complexity);
				util::setValueFromJson(json, "sharpness",	sharpness);
				util::setValueFromJson(json, "regularity",	regularity);
				util::setValueFromJson(json, "orientation", orientation);
				util::setValueFromJson(json, "granularity", granularity);
				util::setValueFromJson(json, "hierarchy",	hierarchy);

				util::setValueFromJson(json, "dominance",	dominance);
				util::setValueFromJson(json, "size",		size);
				util::setValueFromJson(json, "intensity",	intensity);
				util::setValueFromJson(json, "temperature", temperature);
				util::setValueFromJson(json, "contrast",	contrast);
				util::setValueFromJson(json, "saturation",	saturation);

				calc();
			};

		}; using MetaModelRef = std::shared_ptr<MetaModel>;

	}
}