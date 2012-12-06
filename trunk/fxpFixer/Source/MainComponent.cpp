/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent()
{
	addAndMakeVisible(&logger);
	logger.setReadOnly(true);
	logger.setMultiLine(true, true);

    setSize (500, 400);
}

MainContentComponent::~MainContentComponent()
{
	
}

void MainContentComponent::paint (Graphics& g)
{
	g.fillAll (Colours::lightgrey);

    g.setFont (Font (26.0f));
    g.setColour (Colours::black);
    g.drawText ("Drop invalid PitchedDelay .fxp files here",0, 0, getWidth(), 50, Justification::centred, true);
}

void MainContentComponent::resized()
{
	logger.setBounds(50, 70, getWidth()-100, getHeight()-100);
}


bool MainContentComponent::isInterestedInFileDrag (const StringArray& files)
{
	for (int i=0; i<files.size(); ++i)
		if (File(files[i]).getFileExtension().toLowerCase() == ".fxp")
			return true;

	return false;
}

void MainContentComponent::filesDropped (const StringArray& files, int /*x*/, int /*y*/)
{
	for (int i=0; i<files.size(); ++i)
	{
		File f(files[i]);

		if (f.getFileExtension().toLowerCase() == ".fxp")
			processFile(f);
	}
}

void MainContentComponent::processFile(File f)
{
	logger.insertTextAtCaret("Processing " + f.getFileName() + "... ");
	File out(f.getParentDirectory().getChildFile(f.getFileNameWithoutExtension() + "_fixed.fxp"));

	FileInputStream fis(f);
	FileOutputStream fos(out);

	const int blockSize = 8192;
	HeapBlock<char> data(blockSize);

	const int size = (int) fis.getTotalLength();

	for (int i=0; i<size; i += blockSize)
	{
		if (i == 0)
		{
			const int bytesRead = fis.read(data, blockSize);
			const char correct[8] = {0x6c, 0x6b, 0x50, 0x44, 0x00, 0x00, 0x00, 0x66};
			const char wrong[8]   = {0x6c, 0x6b, 0x50, 0x44, 0x00, 0x01, 0x00, 0x00};

			bool doProcess = true;

			for (int i=0; i<8; ++i)
			{
				doProcess &= data[i+16] == wrong[i];
				data[i+16] = correct[i];
			}

			if (! doProcess)
			{
				//AlertWindow::showMessageBox(AlertWindow::NoIcon, "Error", "Error processing "+f.getFileName(), "OK", this);
				logger.insertTextAtCaret("Error!\n");
				return;
			}
			
			fos.write(data, bytesRead);			
		}
		else
		{
			const int bytesRead = fis.read(data, blockSize);
			fos.write(data, bytesRead);
		}
	}
	logger.insertTextAtCaret("OK!\n");

}
