
#include "studio.h"
#include "studio_model.h"

mstudioevent_t* StudioModel::GetEventInfo( int iNumber )
{
	mstudioseqdesc_t* pseqdesc = ExposeSequence();
	
	if (iNumber > pseqdesc->numevents)
		iNumber = 0;
	if (iNumber < 0)
		iNumber = pseqdesc->numevents - 1;
	
	mstudioevent_t* events = (mstudioevent_t*)((byte*)m_pstudiohdr + pseqdesc->eventindex) + iNumber;
	return events;
}

int StudioModel::MaxEvent( void )
{
	mstudioseqdesc_t* pseqdesc = ExposeSequence();
	return pseqdesc->numevents;
}

bool StudioModel::FindEvent(int event)
{
	for (int i = 0; i < EVENT_MAP_SIZE; i++)
	{
		if (event == event_map[i].type)
		{
			event_map_element = i;
			return true;
		}
	}

	event_map_element = 0;
	return false;
}

//////////////////////////////////////////////////////////////////

int event_map_element;

event_map_t event_map[EVENT_MAP_SIZE] =
{
	{ 1000, "Dead From Frame", "" },
	{ 1001, "Uninterruptible From Frame", "" },
	{ 1002, "Interruptible From Frame", "" },
	{ 1003, "Fire Map Trigger", "Trigger Name" },
	{ 1004, "Play WAV File", "WAV File" },
	{ 1005, "Play Sentence", "Sentence" },
	{ 1006, "Don't Send After Sequence", "" }, // ???
	{ 1007, "Play Sequence", "Sequence" },
	{ 1008, "Play WAV File (Voice Channel)", "WAV File" }, // ???
	{ 1009, "Play Random Sentence", "" },
	{ 1010, "Alive From Frame", "" },

	{ 2001, "Monster Drop (Light)", "" },
	{ 2002, "Monster Drop (Heavy)", "" },
	{ 2010, "Monster Swing", "" },

	{ 5001, "Attachment 0 Muzzleflash", "Sprite Type" },
	{ 5002, "Attachment Spark", "" },
	{ 5004, "Play Client-Side WAV File", "WAV File" },
	{ 5011, "Attachment 1 Muzzleflash", "Sprite Type" },
	{ 5021, "Attachment 2 Muzzleflash", "Sprite Type" },
	{ 5031, "Attachment 3 Muzzleflash", "Sprite Type" },

	{ 6001, "Attachment Brass Eject", "" },
};
