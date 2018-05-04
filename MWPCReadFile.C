#include <iostream>
#include <TH1F.h>
#include <TPostScript.h>
#include <TPDF.h>
#include <TCanvas.h>
#include <TSystem.h>

struct SensorHeader
{
  char title[8];
  unsigned char ip;
  unsigned char zero_byte;
  unsigned char version;
  unsigned int trg_num;
};// __attribute__((packed));


struct Board
{
  unsigned char id;
  unsigned int trg_num;
  unsigned int data[15];
};// __attribute__((packed));

struct pcap {
  time_t      tv_sec;
  time_t      tv_usec;
  unsigned int caplen;     /* length of portion present */
  unsigned int len;        /* length this packet (off wire) */
};

int getNBoard(unsigned char Ip)
{

  if( (Ip == 129) || (Ip == 132) )
    return 7;
  if( (Ip == 150) || (Ip == 146) )
    return 4;
  if( (Ip == 135) || (Ip == 136) || (Ip == 137) || (Ip == 138))
    return 6;
  return 5;  
}

void GetOutputFileName(char *outfile, const char * Prefix,
                       const char * infile,const char * Suffix)
{
  char buffer[80];
  sprintf(buffer,"%s",infile);
  char *ptr = strstr(buffer,".raw");
  if( ptr == 0 )
    ptr = strstr(buffer,".dat");
  if( ptr != 0 )
    *ptr = 0;
  sprintf(outfile,"%s%s.%s",Prefix,buffer,Suffix);
}
  
char buffer[2048];
void MWPCReadFile(const char *fname)
{
  FILE *fin = fopen(fname,"rb");
  if(!fin) 
    {
      cout << "No file: " << fname;
      return;
    }
	
  pcap header;

  //TH1F *chamber1 = new TH1F("Chamber1","",192,-0.5,191.5);
  TH1F *chamber1 =  new TH1F("Chamber1", "",192, 5.5,192-0.5);
  TH1F *chamber2 =  new TH1F("Chamber2", "",192,-0.5,192-0.5);
  TH1F *chamber3 =  new TH1F("Chamber3", "",192,-0.5,192-0.5);
  TH1F *chamber4 =  new TH1F("Chamber4", "",192,-0.5,192-0.5);
  TH1F *chamber5 =  new TH1F("Chamber5", "",640,-0.5,640-0.5);
  TH1F *chamber6 =  new TH1F("Chamber6", "",448,-0.5,448-0.5);
  TH1F *chamber7 =  new TH1F("Chamber7", "",576,-0.5,576-0.5);
  TH1F *chamber8 =  new TH1F("Chamber8", "",320,-0.5,320-0.5);
  TH1F *chamber9 =  new TH1F("Chamber9", "",640,-0.5,640-0.5);
  TH1F *chamber10 = new TH1F("Chamber10","",320,-0.5,320-0.5);

  int cnt = 0;
  unsigned int  board_trg_num;

  while(!feof(fin))
    {
      cnt++;
      // if( cnt == 10000000 )
      // 	break;

      if( fread(&header,sizeof(header),1,fin) !=  1 )
	break;
      // cout << header.tv_sec << " " << header.len << endl;
		
      if( fread(buffer,header.len,1,fin) != 1 )
	break;
      char *ptr = buffer;
      ptr+=42;
      if( strncmp(ptr,"NIS_GIBS",8) != 0 )
	break;
      ptr += 8;
      unsigned char Ip = *ptr;
      ptr += 3;
      unsigned int trg_num = *((unsigned int*)ptr);
      ptr += 4;
		
      // cout << int(Ip) << " " << trg_num << endl;
		
      for( int i = 0; i < getNBoard(Ip); i++ )
	{
	  unsigned char bid = ((*ptr)&0xF)-1;
	  // 			unsigned char bid = 1;
	  ptr += 1;
	  board_trg_num = *((unsigned int*)ptr);
	  ptr += 4;
	  // 			board_trg_num = *((unsigned int*)(ptr+1));
	  // 			ptr += (1 + 3*4);
	  // 			unsigned int  *d1 = ((unsigned int*)(ptr));
	  // 			ptr += (1 + 16*4);
	  // 			unsigned int  *d2 = ((unsigned int*)(ptr));
	  // 			ptr += (1 + 16*4);
	  // 			unsigned int  *d3 = ((unsigned int*)(ptr));
	  // 			ptr += (1 + 16*4);
	  // 			unsigned int  *d4 = ((unsigned int*)(ptr));
	  // 			ptr += (1 + 16*4);
	  // 			unsigned int  *d5 = ((unsigned int*)(ptr));
	  // 			ptr += (1 + 16*4);
	  // 			unsigned int  *d6 = ((unsigned int*)(ptr));	
	  // 			ptr += (1 + 16*4);
	  // 			unsigned int  *d7 = ((unsigned int*)(ptr));


			
	  unsigned int  *data = ((unsigned int*)ptr);
	  // 			//
	  // 			// cout << "       " << i << " " << hex <<  int(bid) <<
	  // 			//      << " " << board_trg_num <<  endl;
	  // 			//
	  for( int j = 1; j < 4; j++ )
	    {
	      for( int k = 0 ; k < 32; k++ )
		{
		  //					int k = 16;
		  //					if( ( d1[0]|d1[1]|d2[0]|d2[1]|d3[0]|d3[1]|d4[0]|d4[1]|d5[0]|d5[1]|d6[0]|d6[1]|d7[0]|d7[1] ) != 0 )
		  //					if( ( d1[0]|d1[1]|d2[0]|d2[1]|d3[0]|d3[1]|d4[0]|d4[1]|d5[0]|d5[1]|d6[0]|d6[1] ) != 0 )
		  // 					if( ( d1[0]|d1[1]|d2[0]|d2[1]|d3[0]|d3[1]|d4[0]|d4[1]|d5[0]|d5[1] ) != 0 )
		  if( ( data[j] & (0x1<<k) ) != 0 )
		    {
		      int wire = 0;

		      //*********** Chamber 1 
		      if( Ip == 138 )
			{
			  if( bid < 3 )
			    wire = 2*(32*bid + (31-k));
			  else
			    wire = 2*(32*(5-bid) + (31-k)) + 1;
			  chamber1->Fill(wire);
			}
		      //*********** Chamber 2
		      if( Ip == 137 )
			{
			  if( bid < 3 )
			    wire = 2*(32*bid + k) + 1;
			  else
			    wire = 2*(32*(5-bid) + k);
			  chamber2->Fill(wire);
			}		      
		      //*********** Chamber 3
		      if( Ip == 136 )
			{
			  if( bid < 3 )
			    wire = 2*(32*bid + (31-k));
			  else
			    wire = 2*(32*(5-bid) + (31-k)) + 1;
			  chamber3->Fill(wire);
			}
		      //*********** Chamber 4
		      if( Ip == 135 )
			{
			  if( bid < 3 )
			    wire = 2*(32*bid + k) + 1;
			  else
			    wire = 2*(32*(5-bid) + k);
			  chamber4->Fill(wire);
			}		      
		      //////// Chamber 5 /////////
		      if( Ip == 130 )
			{
			  wire = 2*(32*bid + (31-k));
			  chamber5->Fill(wire);
			}
		      else if( Ip == 131 )
			{
			  wire = 2*(32*(bid+5) + (31-k));
			  chamber5->Fill(wire);
			}
		      else if( Ip == 133 )
			{
			  wire = 2*(32*(bid+5) + (31-k)) + 1;
			  chamber5->Fill(wire);
			}		      
		      else if( Ip == 134 )
			{
			  wire = 2*(32*bid + (31-k)) + 1;
			  chamber5->Fill(wire);
			}
		      //////// Chamber 6 /////////
		      if( Ip == 129 )
			{
			  wire = 2*(32*bid + k);
			  chamber6->Fill(wire);
			}
		      else if( Ip == 132 )
			{
			  wire = 2*(32*bid + k)+1;
			  chamber6->Fill(wire);
			}
		      //*********** Chamber 7
		      if( Ip == 146 )
			{
			  wire = 2*(32*bid + (31-k));
			  chamber7->Fill(wire);
			}
		      else if( Ip == 147 )
			{
			  wire = 2*(32*(bid+4) + (31-k));
			  chamber7->Fill(wire);
			}
		      else if( Ip == 149 )
			{
			  wire = 2*(32*(bid+4) + (31-k)) + 1;
			  chamber7->Fill(wire);
			}
		      else if( Ip == 150 )
			{
			  wire = 2*(32*bid + (31-k)) + 1;
			  chamber7->Fill(wire);
			}
		      //*********** Chamber 8
		      if( Ip == 145 )
			{
			  wire = 2*(32*bid + (31-k));
			  chamber8->Fill(wire);
			}
		      else if( Ip == 148 )
			{
			  wire = 2*(32*bid + (31-k))+1;
			  chamber8->Fill(wire);
			}
		      //*********** Chamber 9  // New mapping
		      if( Ip == 152 )
			{
			  wire = 2*(32*bid + (31-k));
			  chamber9->Fill(wire);
			}
		      else if( Ip == 153 )
			{
			  wire = 2*(32*(bid+5) + (31-k));
			  chamber9->Fill(wire);
			}
		      else if( Ip == 155 )
			{
			  wire = 2*(32*(bid+5) + (31-k)) + 1;
			  chamber9->Fill(wire);
			}
		      else if( Ip == 156 )
			{
			  wire = 2*(32*bid + (31-k)) + 1;
			  chamber9->Fill(wire);
			}
		      //*********** Chamber 10  // New mapping
		      if( Ip == 151 )
			{
			  wire = 2*(32*bid + (31-k));
			  chamber10->Fill(wire);
			}
		      else if( Ip == 154 )
			{
			  wire = 2*(32*bid + (31-k))+1;
			  chamber10->Fill(wire);
			}
		      //cout << "IP = " << Ip << " bid = " << bid << " k = " << k << " wire = " << wire << endl;
		      //
		    } // if( ( data[j] & (0x1<<k) ) != 0 )
		} // for( int k = 0 ; k < 32; k++ )
	    } // for( int j = 1 ; j < 3; j++ )
	  ptr += 60; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  //	  
	} // for( int i = 0; i < getNBoard(Ip); i++ )
		
    } // while
  //
  fclose(fin);
	
  cout << " " << board_trg_num <<  endl;
  //
  char ofile[80];
  // GetOutputFileName(ofile,"",gSystem->BaseName(fname),"ps");
  //TPostScript ps(ofile,111);
  GetOutputFileName(ofile,"",gSystem->BaseName(fname),"pdf");
  TCanvas *c = new TCanvas("c1","c1",600,600);
  c->cd();
  TPDF pdf(ofile,111);
  chamber1->Draw();
  c->Update();
  chamber2->Draw();
  c->Update();
  chamber3->Draw();
  c->Update();
  chamber4->Draw();
  c->Update();
  chamber5->Draw();
  c->Update();
  chamber6->Draw();
  c->Update();
  chamber7->Draw();
  c->Update();
  chamber8->Draw();
  c->Update();
  chamber9->Draw();
  //chamber9->Fit("gaus","R++");
  c->Update();
  // 	chamber10->Draw();
  // 	c->Update();
	
}
