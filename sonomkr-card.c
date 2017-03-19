 /*SoC Driver for The SonoMKR Project Audio Card
 *
 * Author:      Valentin LE BESCOND <valentin.lebescond@sonomkr.fr>
 *              Copyright 2017
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <linux/module.h>
#include <linux/platform_device.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/jack.h>

static const unsigned int wm8782_rates_6144000[] = {
	48000,
};

static struct snd_pcm_hw_constraint_list wm8782_constraints_6144000 = {
	.list = wm8782_rates_6144000,
	.count = ARRAY_SIZE(wm8782_rates_6144000),
};

static int snd_rpi_sonomkr_card_startup(struct snd_pcm_substream *substream)
{
	/* Setup constraints, because there is a 6.144 MHz XTAL on the board */
	snd_pcm_hw_constraint_list(substream->runtime, 0,
				SNDRV_PCM_HW_PARAM_RATE,
				&wm8782_constraints_6144000);
	return 0;
}

static int snd_rpi_sonomkr_card_hw_params(
       struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	int sysclk = 6144000; /* This is fixed on this board */

	/* Set bclk */
	int ret = snd_soc_dai_set_bclk_ratio(cpu_dai,32*2);
	if (ret < 0){
		dev_err(codec->dev,
				"Failed to set BCLK ratio %d\n", ret);
		return ret;
	}

	/* Set sysclk 
	ret = snd_soc_dai_set_sysclk(codec_dai, 1, sysclk, SND_SOC_CLOCK_IN);
	if (ret < 0) {
		dev_err(codec->dev,
				"Failed to set WM8782 SYSCLK: %d\n", ret);
		return ret;
	}
	*/

	return 0;
}

/* machine stream operations */
static struct snd_soc_ops snd_rpi_sonomkr_card_ops = {
       	.startup = snd_rpi_sonomkr_card_startup,
	.hw_params = snd_rpi_sonomkr_card_hw_params,
};

static struct snd_soc_dai_link snd_rpi_sonomkr_card_dai[] = {
{
       .name           = "SonoMKR Card",
       .stream_name    = "SonoMKR Audio Card",
       .cpu_dai_name   = "bcm2708-i2s.0",
       .codec_dai_name = "wm8782",
       .platform_name  = "bcm2708-i2s.0",
       .codec_name     = "wm8782",
       .dai_fmt        = SND_SOC_DAIFMT_I2S |
                         SND_SOC_DAIFMT_NB_NF |
                         SND_SOC_DAIFMT_CBM_CFM,
       .ops            = &snd_rpi_sonomkr_card_ops,
},
};

/* audio machine driver */
static struct snd_soc_card snd_rpi_sonomkr_card = {
       .name           = "SonoMKRCard",
       .owner          = THIS_MODULE,
       .dai_link       = snd_rpi_sonomkr_card_dai,
       .num_links      = ARRAY_SIZE(snd_rpi_sonomkr_card_dai),
};

static int snd_rpi_sonomkr_card_probe(struct platform_device *pdev)
{
       struct device_node *np;
       int ret = 0;

       snd_rpi_sonomkr_card.dev = &pdev->dev;

       np = pdev->dev.of_node;
       if (np) {
               struct snd_soc_dai_link *dai = &snd_rpi_sonomkr_card_dai[0];
               struct device_node *i2s_np;

               i2s_np = of_parse_phandle(np, "i2s-controller", 0);
               if (i2s_np) {
                       dai->cpu_dai_name = NULL;
                       dai->cpu_of_node = i2s_np;
                       dai->platform_name = NULL;
                       dai->platform_of_node = i2s_np;
               }
       }

       ret = snd_soc_register_card(&snd_rpi_sonomkr_card);
       if (ret)
               dev_err(&pdev->dev, "snd_soc_register_card() failed: %d\n",
                       ret);

       return ret;
}

static int snd_rpi_sonomkr_card_remove(struct platform_device *pdev)
{
       return snd_soc_unregister_card(&snd_rpi_sonomkr_card);
}

static const struct of_device_id snd_rpi_sonomkr_card_of_match[] = {
       { .compatible = "sonomkr,card-wm8782", },
       { /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, snd_rpi_sonomkr_card_of_match);

static struct platform_driver snd_rpi_sonomkr_card_driver = {
       .driver = {
               .name           = "snd-sonomkr-card",
               .owner          = THIS_MODULE,
               .of_match_table = snd_rpi_sonomkr_card_of_match,
       },
       .probe  = snd_rpi_sonomkr_card_probe,
       .remove = snd_rpi_sonomkr_card_remove,
};

module_platform_driver(snd_rpi_sonomkr_card_driver);

MODULE_AUTHOR("Valentin LE BESCOND <valentin.lebescond@sonomkr.fr>");
MODULE_DESCRIPTION("ASoC Driver for SonoMKR Audio Card");
MODULE_LICENSE("GPL v2");

