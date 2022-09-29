# Amenland VCV Rack Plugin

Amenland currently features just a single module: Amenator

![Amenator](res/lib/Amenator.png)

## Amenator

The Amenator VCV Rack plugin can be used to output an unlimited amount of variations of the Amen breakbeat. For more information about the Amen break see the [Wikipedia entry on the Amen break](https://en.wikipedia.org/wiki/Amen_break). It takes away the (tedious) task of slicing up the sample and speeding it up to match the song's BPM. It relies on external modules for clock, sequencing and volume envelope. Output is in mono-mox or stereo.

[Click here to watch a brief demonstration of the Amenator](https://youtu.be/UBBTf_88XvE)

### Inputs

+ **BPM**: Clock input. Connect the output of a clock generator. When set to 138bpm, Amenator will produce output at the original music's pace.

+ **Gate**: Gate input. Triggers the playback of the slice selected with the **Slice** input. Usually connected to hte gate output of a (step) sequencer, but could be any other trigger

+ **Slice**: Slice input. The Amen break is three measures long, four-four time. The Amenator internally chops it up into 24 slices. When providing a CV signal between 0 and 2V, Amenator will select the slice proportional to the supplied voltage for playback triggered by **Gate**. This voltage range corresponds to octaves C4 and C5 in V/Oct scale, so it makes sense to connect a V/Oct output of any suitable module to this input.

+ **Env**: Envelope input, optional. This is used to control the volume envelope of the Amenator's output. Connect the output of an envelope generator to control the volume envelope of the output. A common pattern is to also connect the output that is connected to the Amenators **Gate** input the enevelope generator's gate input.

### Outputs

+ **L/M**: Left or Mono-mix. Outputs sample's left channel, starting at **Slice** when **Gate** is triggered. When **R** is not connected, it provides a mono-mix under the same conditions.

+ **R**: Right. Outputs sample's right channel, starting at **Slice** when **Gate** is triggered

+ **?**: There are no other outputs. Or are there?

## Radio Zwijn
The Amenland plugin and the Amenator are brought to you by [Radio Zwijn](https://www.youtube.com/channel/UCq4lQI_iIkHyHzWCdkAC6Fg). Thanks to my buddies at Radio Zwijn for providing me with a feedback and various valuable assets used in the production!

## Licenses
All source code in this repository is copyright © 2022 Jan Willem Penterman/Genoil and licensed under GNU GPLv3

All graphics including the Radio Zwijn logo, panels and components are copyright © 2022 Jan Willem Penterman/Genoil and may not be used in derivative works.

## Donate
Altough the Amenator is provided completely free of charge, you're welcome to buy me a beer:

[![Donate](res/lib/btn_donateCC_LG.gif)](https://www.paypal.com/donate/?business=B6FVZKEP4SG66&no_recurring=1&item_name=a.k.a.+Genoil%2C+for+his+work+on+the+Amenator%21&currency_code=EUR)

