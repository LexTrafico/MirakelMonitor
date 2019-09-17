#pragma once

typedef	short s_int16;

/* SELECTIEVE DETECTIE */
/* ------------------- */

/* codering van de elementen */
/* ------------------------- */
#define CIF_DSI_LUS	  0	/* lusnummer				*/
#define CIF_DSI_VTG	  1	/* voertuigcategorie			*/
#define CIF_DSI_LYN	  2	/* lijnnummer				*/
#define CIF_DSI_WDNST  3	/* wagendienstnummer			*/
#define CIF_DSI_BEDR	  4	/* bedrijfsnummer			*/
#define CIF_DSI_NUM	  5	/* voertuignummer			*/
#define CIF_DSI_DIR	  6	/* richtingsnummer			*/
#define CIF_DSI_STAT	  7	/* voertuigstatus			*/
#define CIF_DSI_PRI	  8	/* prioriteitsklasse			*/
#define CIF_DSI_STP	  9	/* stiptheidsklasse			*/
#define CIF_DSI_TSTP	 10	/* stiptheid [s]			*/
#define CIF_DSI_LEN	 11	/* voertuiglengte [m]			*/
#define CIF_DSI_SPD	 12	/* voertuigsnelheid [m/s]		*/
#define CIF_DSI_LSS	 13	/* afstand tot stopstreep [m]		*/
#define CIF_DSI_TSS	 14	/* passeertijd stopstreep [s]		*/
#define CIF_DSI_RIT	 15	/* ritnummer				*/
#define CIF_DSI_RITC	 16	/* ritcategorie				*/
#define CIF_DSI_ROUT	 17	/* routenummer openbaar vervoer		*/
#define CIF_DSI_TYPE	 18	/* type melding				*/
#define CIF_DSI_OVC	 19	/* codering meldpunt openbaar vervoer   */
#define CIF_DSI_XGRAD 20	/* breedtegraad graden			*/
#define CIF_DSI_XMIN	 21	/* breedtegraad minuten			*/
#define CIF_DSI_XSEC	 22	/* breedtegraad seconden		*/
#define CIF_DSI_XHSEC 23	/* breedtegraad honderdste seconden	*/
#define CIF_DSI_YGRAD 24	/* lengtegraad graden			*/
#define CIF_DSI_YMIN	 25	/* lengtegraad minuten			*/
#define CIF_DSI_YSEC	 26	/* lengtegraad seconden			*/
#define CIF_DSI_YHSEC 27	/* lengtegraad honderdste seconden	*/
#define CIF_DSI_JAAR	 28	/* jaartal				*/
#define CIF_DSI_MND	 29	/* maand				*/
#define CIF_DSI_DAG	 30	/* dag					*/
#define CIF_DSI_UUR	 31	/* uur					*/
#define CIF_DSI_MIN	 32	/* minuten				*/
#define CIF_DSI_SEC	 33	/* seconden				*/
#define CIF_DSI_RES1	 34	/* reserve 1				*/
#define CIF_DSI_RES2	 35	/* reserve 2				*/

/* afwijkende defaultwaarden */
/* ------------------------- */
#define CIF_DSI_TSTP_DEF  3600	/* defaultwaarde CIF_DSI_TSTP	*/ 
#define CIF_DSI_LSS_DEF   9999	/* defaultwaarde CIF_DSI_LSS	*/
#define CIF_DSI_TSS_DEF    255	/* defaultwaarde CIF_DSI_TSS	*/

/* voertuigcategorie */
/* ----------------- */
#define CIF_BUS	  1	/* bus					*/
#define CIF_TRAM       2	/* tram					*/ 
#define CIF_POL	  3	/* politie				*/ 
#define CIF_BRA	  4	/* brandweer				*/
#define CIF_AMB	  5	/* ambulance				*/ 
#define CIF_CVV	  6	/* collectief vraagafhankelijk vervoer 	*/ 
#define CIF_TAXI	  7	/* taxi					*/
#define CIF_URO	 99	/* onbekend voertuig			*/

/* richting */
/* -------- */
#define CIF_RAF      201	/* rechtsaf				*/
#define CIF_LAF      202	/* linksaf				*/
#define CIF_RDR      203	/* rechtdoor				*/

/* voertuigstatus */
/* -------------- */
#define CIF_RIJD	  1	/* rijden				*/
#define CIF_HALT   	  2	/* halteren				*/
#define CIF_START  	  3	/* vertrekmelding			*/
#define CIF_STOP   	  4	/* stilstand				*/

/* prioriteit */
/* ---------- */
#define CIF_PRI1	  1	/* geen prioriteit/alleen aanvraag	*/
#define CIF_PRI2	  2	/* geconditioneerde prioriteit		*/ 
#define CIF_PRI3	  3	/* absolute prioriteit			*/
#define CIF_SIR	  4	/* sirene/zwaailicht			*/

/* stiptheid */
/* --------- */
#define CIF_TE_LAAT    1	/* te laat				*/
#define CIF_OP_TIJD 	  2	/* op tijd				*/
#define CIF_TE_VROEG	  3	/* te vroeg				*/

/* ritcategorie */
/* ------------ */
#define CIF_DIENST  	 10	/* dienstregelingsrit			*/
#define CIF_MAT     	 11	/* materieelrit				*/
#define CIF_MATIN   	 12	/* materieelrit inrukrit naar remise	*/ 
#define CIF_MATUIT  	 13	/* materieelrit uitrukrit van remise	*/

/* type melding */
/* ------------ */
#define CIF_DSIN	  1	/* inmelding				*/
#define CIF_DSUIT	  2	/* uitmelding				*/
#define CIF_DSVOOR	  3	/* voormelding				*/


extern const char * FC_code[];
extern const char * SCH_code[];
extern const char * PRM_code[];
extern const char * H_code[];
extern const char * IS_code[];
extern const char * US_code[];
extern const char * T_code[];
extern const char * C_code[];
extern const char * D_code[];
extern const char * MM_code[];

extern short PAR[];
extern short VG[];

extern short A[];
extern short AA[];
extern short YV[];
extern short SG[];
extern short EGL[];
extern short CG[];
extern short K[];
extern short RV[];
extern short WG[];
extern short TVG[];
extern short TFG[];

extern short D[];
extern short SD[];
extern short ED[];
extern short DB[];
extern short TDB[];
extern short TDB_timer[];
extern short * TDB_max;
extern short TBG[];
extern short TBG_timer[];
extern short * TBG_max;
extern short TOG[];
extern short TOG_timer[];
extern short * TOG_max;
extern short CIF_KLOK[];
#define CIF_UUR	    4		/* uur				*/
#define CIF_MINUUT	    5		/* minuut			*/
#define CIF_SECONDE	    6		/* seconde			*/
extern const short *DS_code[];
extern short CIF_IS[];
extern short CIF_GUS[];
extern short CIF_WUS[];
extern short T[];
extern short IT[];
extern short RT[];
extern short HT[];
extern short ET[];
extern short T_timer[];
extern short * T_max;

extern short MK[];
extern short EG[];
extern short RW[];
extern short TRG[];
extern short GL[];
extern short CV[];
extern short PG[];
extern short RC[];
extern short DEC[];
extern short INC[];

extern short CIF_DSI[];
extern short CIF_DSIWIJZ;

extern short TS;
extern short TE;
extern short TM;

extern short VS[];
extern short FG[];
extern short MG[];
extern short RR[];
extern short ST[];

extern short MM[];
extern short C[];
extern short C_counter[];
extern short *C_max;
extern short R[];
extern short TFB_timer[];
extern short AR[];
extern short PR[];
extern short G[];
extern short AAPR[];
extern short Z[];
extern short RA[];
extern short PFPR[];

extern short X[];
extern short YM[];

extern short * SCH;
extern short H[];
extern short IH[];
extern short EH[];
extern short SH[];
extern short TDH[];
extern short * TDH_max;
extern short TDH_timer[];
extern short * TVG_max;
extern short TVG_timer[];
extern short * TFG_max;
extern short TFG_timer[];

extern short * PRM;
extern short FM[];
extern short TFL[];

extern short OG[];
extern short BG[];
extern short FL[];


extern char ML;
extern char SML;
extern char DBOG;

extern s_int16 CIF_DSIWIJZ;

extern short * TFL_max;
extern short TFL_timer[];
extern short CFL_counter[];
extern short * CFL_max;

#define IS  CIF_IS
#define US  CIF_WUS
#define GUS CIF_GUS

#define CIF_DET_BEZET        1
#define CIF_DET_STORING      2  
#define CIF_DET_BOVENGEDRAG  4
#define CIF_DET_ONDERGEDRAG  8

#define ML1 0
#define ML2 1
#define ML3 2
#define ML4 3
#define ML5 4
#define ML6 5
#define ML7 6
#define ML8 7

extern short FC_MAX;
extern short PRM_MAX;
extern short SCH_MAX;
extern short TM_MAX;
extern short CT_MAX;
extern short DP_MAX;
extern short HE_MAX;
extern short IS_MAX;
extern short US_MAX;
extern short ME_MAX;

#define _CIF_JAAR			0		
#define _CIF_MAAND			1		
#define _CIF_DAG			2
#define _CIF_DAGSOORT		3	
#define _CIF_UUR			4
#define _CIF_MINUUT			5		
#define _CIF_SECONDE	    6		
#define _CIF_SEC_TELLER		7		
#define _CIF_TSEC_TELLER	8

#define _CG_RA  0
#define _CG_VS  1
#define _CG_FG  2
#define _CG_WG  3
#define _CG_VG  4
#define _CG_MG  5
#define _CG_GL  6
#define _CG_RV  7
#define _CG_NO  8
#define _CG_BG  9
#define _CG_MAX 10   

extern int CCOL_Time_Speed_Halt;

extern short CIF_PARM1WIJZPB;
extern short CIF_PARM1WIJZAP;
extern short CIF_PARM2WIJZPB;
extern short CIF_PARM2WIJZAP;
