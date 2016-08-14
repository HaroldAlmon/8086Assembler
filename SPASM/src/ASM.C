/************************************************************************
*                                                                       *
*               Project #2                                              *
*                                                                       *
*************************************************************************
*
* Name:         Harold Almon
*
* Class:		Introduction to Systems Programming
*
* Title:		SPASM Assembler - Pass 1.
*
* Description:	This program assembles a program written in SPASM
*				source code format.  A listing is produced in
*				<file_spec>.lst
*
*/

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*						  Include Files 									 */
/*                                                                           */
/*---------------------------------------------------------------------------*/

	#include <ctype.h>
	#include <malloc.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                        Constants                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

	/***************
		Operand 1
	****************/

	#define UNKNOWN_TYPE 0
	#define	REG_A		1
	#define	REG_B		2
	#define	REG_X		3
	#define REG_Y		4

	/***************
		Operand 2
	****************/

	#define	IMMED		10
	#define	DIR_ADDR	10
	#define INDIR_ADDR	12
	#define	INDIR_A		13
	#define	INDIR_B		14
	#define INDIR_X		15
	#define INDIR_Y		16
	#define QUOTE		17

	/********************
		Operand 2 Type
	*********************/

	#define	BYTE_ABS	1
	#define BYTE_REL	2
	#define	WORD_ABS	3
	#define WORD_REL	4

/**	#define	LOC_CTR		12
	#define LABELS		35
	#define OP_CODE		45
	#define	OPERAND1	55
	#define OPERAND2	65 **/

	#define	LOC_CTR		16
	#define LABELS		29
	#define OP_CODE		39
	#define	OPERAND1	49
	#define OPERAND2	59

	#define	TRUE			1
	#define FALSE			0
	#define SYMBOLS_NUM		100		/* size of the symbol table */
	#define	MAX_TOKENS		20		/* maximum tokens parsed on a line */
	#define	MAX_TOKEN_LEN	81		/* the biggest size of a token */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                        Structure Declarations                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

    struct instr_info
	{
		char	*name;
		int		no_operands;
		int		operand_1;
		int		operand_2;
		int		operand_type;
		char	*mach_code;
	};

	struct error_message_struct
	{
		struct	error_message_struct 	*next_msg;
		char							error_text[81];
	};

	struct source_lst_struct
	{
		struct	error_msg_struct		*message;
		char							src_text[81];
	};

	struct sym_table_struct
	{
		char	name[MAX_TOKEN_LEN + 1];
		int		value;
		int		line;			/* line number in source code of symbol */
	};

	struct token_info_struct
	{
		char	*token;
		int		position;
		int		length;
		int		token_type;
	};

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                        Data Structures                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

struct instr_info instruction_table[100] =
{
/*
	Mnemonic	Number	Oper_1		Oper_2			Operand 		Machine
													type			Code
*/
	";",		0,		0,			0,				0,				NULL,
	"DB",		0,		0,			0,				0,				NULL,
	"DS",		1,		IMMED,		0,				0,				NULL,
	"ORG",		1,		IMMED,		0,				0,				NULL,
	"EQU",		1,		IMMED,		0,				0,				NULL,
	"END",		0,		0,			0,				0,				NULL,
	"ADD",		2,		REG_A,		IMMED,			BYTE_ABS,		"04",
	"ADD",		2,		REG_A,		INDIR_ADDR,		WORD_ABS,		"0206",
	"ADD",		2,		REG_A,		INDIR_Y,		0,				"0204",
	"ADD",		2,		REG_B,		IMMED,			BYTE_ABS,		"80C4",
	"ADD",		2,		REG_B,		INDIR_ADDR, 	WORD_ABS,		"0226",
	"ADD",		2,		REG_A,		REG_B,			0,				"02C4",
	"INT",		1,		IMMED,		0,				BYTE_ABS,		"CD",
	"INX",		0,		0,			0,				0,				"42",
	"INY",		0,		0,			0,				0,				"46",
	"JZ",		1,		DIR_ADDR,	0,				BYTE_REL,		"74",
	"JNZ",		1,		DIR_ADDR,	0,				BYTE_REL,		"75",
	"JL",		1,		DIR_ADDR,	0,				BYTE_REL,		"7C",
	"JG",		1,		DIR_ADDR,	0,				BYTE_REL,		"7F",
	"JMP",		1,		DIR_ADDR,	0,				WORD_REL,		"E9",
/*
	Mnemonic	Number	Oper_1		Oper_2			Operand 		Machine
													type			Code
*/
	"LDA",		2,		REG_A,		IMMED,			BYTE_ABS,		"B0",
	"LDA",		2,		REG_A,		INDIR_ADDR, 	WORD_ABS,		"A0",
	"LDA",		2,		REG_A,		INDIR_Y,		0,				"8A04",
	"STA",		2,		REG_A,		INDIR_ADDR, 	WORD_ABS,		"A2",
	"STA",		2,		REG_A,		INDIR_Y,		0,				"8804",
	"LDA",		2,		REG_B,		IMMED,			BYTE_ABS,		"B4",
	"LDA",		2,		REG_B,		INDIR_ADDR, 	WORD_ABS,		"8A26",
	"STA",		2,		REG_B,		INDIR_ADDR, 	WORD_ABS,		"8826",
	"LDX",		2,		REG_X,		INDIR_ADDR, 	WORD_ABS,		"8B16",
	"LDX",		2,		REG_X,		DIR_ADDR,		WORD_ABS,		"BA",
	"LDX",		2,		REG_Y,		INDIR_ADDR, 	WORD_ABS,		"8B36",
	"LDX",		2,		REG_Y,		DIR_ADDR,		WORD_ABS,		"BE",
	"NEG",		1,		REG_A,		0,				0,				"F6D8",
	"NEG",		1,		REG_B,		0,				0,				"F6DC",

	/******************
		End of table
	*******************/

	NULL,		0,		0,			0,				0,				NULL
};
	int		end_flag;
	int		location_ctr;
	FILE 	*out_file;
	struct	sym_table_struct	symbol_table[ SYMBOLS_NUM ];
	int		src_line_ctr;
	struct	source_lst_struct	source_lst[ 200 ];

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                        Function Prototypes                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/

	void	add_to_lst					( struct token_info_struct (* line_tokens)[MAX_TOKENS], int op_code_idx);
	unsigned int asc_to_int				( char *ascii_text );
	int		check_syntax				( struct token_info_struct (* line_tokens)[MAX_TOKENS]);
	int		fixup_code					( void );
	int		free_tokens					( struct token_info_struct (* line_tokens)[MAX_TOKENS] );
	int		generate_code				( int instr_match, struct token_info_struct (* line_tokens)[MAX_TOKENS] );
	int		initialize_data_structures	( int argc, char *argv[], FILE **in_file);
	int		hash_name					( char *name );
	int		identify_token				(char *token);
	int     main                        ( int argc, char *argv[] );
	void	output_symbols				(void);
	struct	token_info_struct (* parse_line ( char *in_buf ))[MAX_TOKENS];
	int		perform_pass_one			( FILE *in_file );
	void	pseudo_op( char *instruction, int op_code_idx,
					struct token_info_struct (* line_tokens)[MAX_TOKENS] );
	int		store_symbol                ( char *symbol, int symbol_value, int source_line );
	struct	token_info_struct *get_token( char *src_ptr, int column );

/*****************************************************************************
*                                                                            *
*           main()                                                           *
*                                                                            *
******************************************************************************
*
* AUTHOR:	Harold Almon
*
* FUNCTION: This procedure assembles a SPASM format source code file.
*
* INPUTS:	argv	->	array of pointers to the command line parameters
*			argc	->	number of command line parameters.
*
* OUTPUTS:	Creates a listing file in <file>.lst
*			Creates a linked object in <file>.com
*
* RETURNS:	Returns zero is no error.
*			Returns non-zero vale if an error occurs.
*/
main( int argc, char *argv[] )
{
	static FILE	*in_file;
	if(
		initialize_data_structures(argc, argv, &in_file ) ||
		perform_pass_one( in_file ) ||
		fixup_code()
	)
	{
		puts("Assembly contains errors");
	}
	else
	{
		puts("Assembly Complete");
	}
	fixup_code();
	if( out_file )
		fclose( out_file );
	return 0;
}

/*****************************************************************************
*                                                                            *
*           add_to_lst()                                                     *
*                                                                            *
******************************************************************************
*
* AUTHOR:	Harold Almon
*
* FUNCTION:	This procedure adds a line to the source listing file.
*
* INPUTS:	line_tokens	->	array of line tokens in this line
*			op_code_idx	->	token no. of the operand in this line.
*
* OUTPUTS:	Adds a line to the source code listing source_lst[].
*
* RETURNS:	None.
*/
void add_to_lst( struct token_info_struct (* line_tokens)[MAX_TOKENS],int op_code_idx)
{
	int		column_idx;
	char	location_ascii[5];	/* location counter in asciiz format */

	/**************************************
		Copy blanks into the source line
	***************************************/

	source_lst[src_line_ctr].src_text[0] = ' ';
	for( column_idx = 1; column_idx < 79; column_idx++ )
	{
		source_lst[src_line_ctr].src_text[column_idx] =
			source_lst[src_line_ctr].src_text[column_idx-1];
	}
	source_lst[src_line_ctr].src_text[79] = '\n';

	/**********************************************
		Copy the Label to the source code listing
	***********************************************/

	if( (*line_tokens)[0].position == 1 )	/* is the 1st token a label ? */
	{
		strncpy( source_lst[src_line_ctr].src_text+LABELS,
			(*line_tokens)[0].token,
			(*line_tokens)[0].length);
	}


	/**************************************************
		Copy the opcode, operand 1, operand 2, and
		the location counter into the source listing
	***************************************************/

	strncpy( source_lst[src_line_ctr].src_text+OP_CODE,
		(*line_tokens)[op_code_idx].token,
		(*line_tokens)[op_code_idx].length);

	if( (*line_tokens)[op_code_idx+1].token )
		strncpy( source_lst[src_line_ctr].src_text+OPERAND1,
			(*line_tokens)[op_code_idx+1].token,
			(*line_tokens)[op_code_idx+1].length);

	if( (*line_tokens)[op_code_idx+3].token )
		strncpy( source_lst[src_line_ctr].src_text+OPERAND2,
			(*line_tokens)[op_code_idx+3].token,
			(*line_tokens)[op_code_idx+3].length);

	sprintf( location_ascii, "%.04x", location_ctr);
	strncpy( source_lst[src_line_ctr].src_text+LOC_CTR,
		strupr( location_ascii ), 4 );

	if( fputs( source_lst[src_line_ctr].src_text, out_file) == EOF )
	{
		fprintf( stderr, "*** SPASM: ERROR: cannot write output file\n");
	}
	/**** printf( "%s\n", source_lst[src_line_ctr].src_text );	/* print the source listing */
}

/*****************************************************************************
*                                                                            *
*           asc_to_int()                                                     *
*                                                                            *
******************************************************************************
*
* AUTHOR:	Harold Almon
*
* FUNCTION:	This procedure converts a decimal or hexadecimal ASCII
*			string into an integer.  Hexadecimal strings are indicated
*			by placing an upper case 'H' at the end of the string.
*
* INPUTS:	ascii_text	->	string to be converted
*
* OUTPUTS:	None.
*
* RETURNS:	The connverted number as an integer.
*/
unsigned int asc_to_int( char *ascii_text )
{
	unsigned int		result = 0;
	if( ascii_text[ strlen( ascii_text ) - 1 ] == 'H')
	{
		/***********************************
			Convert Hex number to integer
		************************************/

		for( ; *ascii_text && *ascii_text != 'H'; ascii_text++ )
		{
			result *= 16;
			if( *ascii_text >= '0' && *ascii_text <= '9' )
			{
				result += *ascii_text & 0x0F;
			}
			else
			if( *ascii_text >= 'A' && *ascii_text <= 'F' )
			{
				result += *ascii_text - 'A' + 10;
			}
			else
			{
				result = 0;
				break;		/* invalid character - return */
			}
		}
	}
	else
	{
		/***************************************
			Convert Decimal number to integer
		****************************************/

		result = atoi( ascii_text );
	}
	return result;
}

/*****************************************************************************
*                                                                            *
*           check_syntax()                                                   *
*                                                                            *
******************************************************************************
*
* AUTHOR:	Harold Almon
*
* FUNCTION:	This procedure determines if the current line of source code
*			has a valid syntax.
*
* INPUTS:	line_tokens	->	array of line tokens in this line
*
* OUTPUTS:
*
* RETURNS:	0 if INVALID syntax
*			a non-zero pointer to an instruction template that has
*			a syntax which matched the current line.
*
*/
int check_syntax( struct token_info_struct (* line_tokens)[MAX_TOKENS] )
{
	int i;
	int		address_length;
	int		good_token_idx;
	int		instr_idx;			/* index into the instruction table */
	int		operand1;
	int		operand2;
	int		operand_type;
	int		op_code_idx;		/* index into the line_tokens */

/*	for( i=0; i<6; i++)
	{
		if( (*line_tokens)[i].token[0] != '\0')
			printf("%10.10s   ", (*line_tokens)[i].token);
	}
	puts(""); ***/

	op_code_idx = 0;						/* assume op code is in first token */
	if( (*line_tokens)[0].token[0] == ';' )	/* is the 1st token a comment ? */
	{										/* if YES then */
		instr_idx = 1;						/* first entry is for a comment */
		goto exit_check_syntax;				/* return */
	}

	/***********************************
		Check for a label in column 1
	************************************/

	if( (*line_tokens)[0].position == 1)				/* Must start in col 1 */
	{
		if( (* line_tokens)[0].token_type == DIR_ADDR )	/* Must be an address */
		{
			/********************************
				Add symbol to symbol table
			*********************************/

			if( strlen( (* line_tokens)[0].token) >= 9 )
			{
				fputs("*** SPASM: WARNING: Label truncated to 8 characters\n", out_file);
				(* line_tokens)[0].token[8] = '\0';
				(* line_tokens)[0].length = 8;
			}
        	if( strcmp( (*line_tokens)[1].token, "EQU" ) ) /* instr must not be "EQU" */
				store_symbol( (* line_tokens)[0].token, location_ctr, src_line_ctr );
			op_code_idx = 1;					/* 2nd token must be the opcode */\
		}
		else
			fputs("*** SPASM: ERROR: Illegal token in column 1\n", out_file);
	}

	/*****************************
		Identify the strings in
		operand 1 and operand 2
	******************************/

	if( (*line_tokens)[op_code_idx+1].token[0] != '\0' )
		operand1 = (*line_tokens)[op_code_idx+1].token_type;
	else
		operand1 = 0;
	if( (*line_tokens)[op_code_idx+3].token[0] != '\0' )
		operand2 = (*line_tokens)[op_code_idx+3].token_type;
	else
		operand2 = 0;

	/************************************
		Add line to source code listing
	*************************************/

	add_to_lst( line_tokens, op_code_idx );

	/******************************************
		Search the insrtuction table for a
		valid match in the instruction table
	*******************************************/

	good_token_idx = 0;		/* assume no good tokens */
	for( instr_idx = 1; instruction_table[ instr_idx - 1].name; instr_idx++ )
	{
		/**************************************
			Does the instrcution name match?
		***************************************/

		if( !strcmp( (*line_tokens)[op_code_idx].token,
						instruction_table[instr_idx - 1].name ) )
		{
			if( !good_token_idx )		/* if first match then ... */
				good_token_idx = 1;		/* the first token (operator) matches */
			if( instruction_table[instr_idx - 1].no_operands == 0)
			{
				break;		/* no operands, valid instruction */
			}

			/**********************************
				Does the first operand match?
			***********************************/

			if( operand1 == instruction_table[instr_idx - 1].operand_1 )
			{
				good_token_idx = 2;	/* the 2nd token (operand 1) matches */
				if( instruction_table[instr_idx - 1].no_operands == 1)
				{
					break;	/* one operand, valid instruction */
				}

				/***********************************************
					Check for a comma between operand 1 and 2
				************************************************/

				if( *( (* line_tokens)[op_code_idx + 2].token ) != ',')
				{
					fputs("*** SPASM: ERROR: Missing comma between operands\n", out_file);
					break;
				}

				/************************************
					Does the second operand match?
				*************************************/

				if( operand2 == instruction_table[instr_idx - 1].operand_2 )
					break;	/* two operands, valid instruction */
			}
		}
	}

	/************************************
		Now check if a match was found
	*************************************/

	if( !instruction_table[instr_idx - 1].name )
	{
		/****************************
			if no match found then
		*****************************/

		switch( good_token_idx )
		{
			case 2:
				fputs( "*** SPASM: ERROR: Invalid second operand\n", out_file );
				break;

			case 1:
				fputs( "*** SPASM: ERROR: Invalid first operand\n", out_file );
				break;

			default:
				fputs( "*** SPASM: ERROR: Invalid instruction\n", out_file );
				break;
		}
		instr_idx = 0;			/* indicate invalid instruction */
	}
	else
	{
		/**************************************************
			Now increment the location counter based
			on the operand type found in the instruction
		***************************************************/

		address_length = 0;
		operand_type = 0;
		operand_type = instruction_table[instr_idx - 1].operand_type;

		if( operand_type == BYTE_ABS || operand_type == BYTE_REL )
			address_length = 1;

		if( operand_type == WORD_ABS || operand_type == WORD_REL )
			address_length = 2;

		location_ctr +=
			strlen( instruction_table[instr_idx - 1].mach_code ) / 2 +
			address_length;

		/*******************************
			Process Pseudo operations
		********************************/

		pseudo_op( instruction_table[instr_idx - 1].name, op_code_idx, line_tokens );
	}
	src_line_ctr++;		/* increment source line counter */

exit_check_syntax:
	free_tokens( line_tokens );
	return instr_idx;			/* return a pointer to the matching instruction */
}

/*****************************************************************************
*                                                                            *
*           fixup_code()                                                     *
*                                                                            *
******************************************************************************
*
* AUTHOR:	Harold Almon
*
* FUNCTION:
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*/
int fixup_code( void )
{
	return 0;
}

/*****************************************************************************
*                                                                            *
*           free_tokens()                                                    *
*                                                                            *
******************************************************************************
*
* AUTHOR:	Harold Almon
*
* FUNCTION:	This procedure goes through the array of structures
*			that holds the tokens created by this program
*			and releases the memory used to hold the tokens.
*
* INPUTS:	line_token	->	array of token structures
*
* OUTPUTS:	Frees the memory that holds the tokens.
*
* RETURNS:	0.
*/
int free_tokens( struct token_info_struct (* line_tokens)[MAX_TOKENS] )
{
	int		token_ctr;
	for( token_ctr = 0 ;
		 token_ctr < MAX_TOKENS &&
		 (* line_tokens)[ token_ctr ].token	/* check for NULL pointer */
		 ; token_ctr++ )
	{
		free( (* line_tokens)[ token_ctr ].token );
	}
	return 0;
}

int generate_code( int instr_match, struct token_info_struct (* line_tokens)[MAX_TOKENS] )
{
	return 0;
}

/*****************************************************************************
*                                                                            *
*           get_token()                                                      *
*                                                                            *
******************************************************************************
*
* AUTHOR:	Harold Almon
*
* FUNCTION:	This procedure extracts a token from a line of ASCII characters.
*
* INPUTS:	src_ptr	-> a pointer to an ASCII string
*			column	-> the current position in the ASCII string of src_ptr
*
* OUTPUTS:	None.
*
* RETURNS:	A pointer to a structure which contains:
*
*				o a pointer to a copy of the token
*				o the position of the token in the original string
*				o the length of the token
*/
struct token_info_struct *get_token( char *src_ptr, int column )
{
	struct	token_info_struct *return_code;
	static	struct token_info_struct	token_info;
	char	*token_buf;					/* pointer a token_info_struct */
	int		token_idx;
	char	token[MAX_TOKEN_LEN];

	/********************************************
		Move to the first non-white character
		in the ASCII string
	*********************************************/

	while( *src_ptr == ' ' || *src_ptr == '\t' ) /* skip any white spaces */
	{
		src_ptr++;					/* advance source pointer */
		column++;					/* increment column counter */
	}
	/*************************
		Check for a comment
	**************************/

	token_info.position = column;
	if( *src_ptr == ';' || *src_ptr == ',')		/* check for single character tokens */
	{
		token[0] = *src_ptr;				/* copy token */
		token[1] = '\0';					/* mark the string end */
		token_idx = 1;
		token_info.length = 1;				/* set the length to 1 */
	}
	else									/* the token is NOT a comment */
	{
		/*****************************************
			Copy the token to a new buffer area
		******************************************/

		token[0] = '\0';					/* reset token string */
		token_idx = 0;						/* reset token length counter */

		/******************************
			Check for a quoted string
		*******************************/

		if( *src_ptr == '\'' )		/* is this character a single quote? */
		{							/* if YES then */
			token[ token_idx++ ] = *src_ptr++;	/* copy character to token */
			for( ;
				   *src_ptr != '\'' &&	/* end of token is ' */
				   *src_ptr != '\n' &&	/* or a new-line character */
				   !iscntrl( *src_ptr ) && /* reject control characters */
				   token_idx < MAX_TOKEN_LEN /* check length of token */
				   ; src_ptr++, token_idx++)
			{
				token[ token_idx ] = *src_ptr;	/* copy character to token */
			}
			if( *src_ptr == '\'' )
			{
				token[ token_idx++ ] = *src_ptr++;	/* copy the end quote to token */
			}
			else
			{
				fputs("*** SPASM: ERROR: End quote missing in statement\n", out_file);
			}
		}
		else
			for( ;
				   *src_ptr != ' ' &&	/* end of token is ' ' */
				   *src_ptr != ',' &&	/* or a comma */
				   *src_ptr != '\t'&&	/* or a tab */
				   *src_ptr != '\n' &&	/* or a new-line character */
				   !iscntrl( *src_ptr ) && /* reject control characters */
				   token_idx < MAX_TOKEN_LEN /* check length of token */
				   ; src_ptr++, token_idx++)
			{
				if( isalpha( (int) *src_ptr ) )
				{
					*src_ptr = (char) toupper( (int) *src_ptr);
				}
				token[ token_idx ] = *src_ptr;	/* copy character to token */
			}
		token_info.length = token_idx;	/* record length of token */
		token[ token_idx ] = '\0';		/* mark the end of the token */
	}

	if( token_idx == 0 )
	{
		token_info.token = NULL;		/* store NULL pointer in structure */
		return_code = NULL;
		goto exit_get_token;
	}
	/***************************************
		Allocate memory to hold the token
	****************************************/

	if( (token_buf = (char *)
			malloc( sizeof (token) + 1 )) )
		token_info.token = token_buf;			/* copy pointer into structure */
	else
	{
		fprintf( stderr, "ERROR:Memory allocation error\n");
		exit(-1);
	}

	strcpy( token_buf, token );			/* copy token into buffer */
	token_info.token_type = identify_token( token );
	return_code = &token_info;

exit_get_token:
	return return_code;
}

/*****************************************************************************
*                                                                            *
*           hash_name()                                                      *
*                                                                            *
******************************************************************************
*
* AUTHOR:	Harold Almon
*
* FUNCTION:	This procedure takes an ASCII string hashes it and returns
*			a hash value from 0 to 99. If c is a character string then:
*
*			hash vale = 1*c[1] + 10*c[2] + 100*c[3]
*						1*c[4] + 10*c[5] + 100*c[6] ...
*
* INPUTS:	name	->	ASCII string to be hashed
*
* OUTPUTS:	None.
*
* RETURNS:	Hash value from 0 to 99.
*/
int hash_name( char *name )
{
	static	hash_value = 0;
	int		multiplier = 1;
	while( *name != '\0' )			/* while not end of string */
	{
		hash_value = (int) *name++ * multiplier;
		if( multiplier == 100 )		/* check for multiplier overflow */
		{
			multiplier = 1;			/* reset multiplier */
		}
		else
		{
			multiplier *= 10;		/* increase multiplier by a factor of 10 */
		}
	}
	hash_value %= 100;				/* take the modulus of the hash value */
	return hash_value;
}

/*****************************************************************************
*                                                                            *
*           identify_token()                                                 *
*                                                                            *
******************************************************************************
*
* AUTHOR:	Harold Almon
*
* FUNCTION:	This procedure determines the type of a token such as
*			direct address, indirect address, quoted string and so forth.
*
* INPUTS:	token	->	pointer to a token
*
* OUTPUTS:	None.
*
* RETURNS:	The token type (see eqautes at the beginning of this file).
*/
int identify_token(char *token)
{
	char	tchar;					/* token character */
	int		token_idx;				/* token index */
	static	struct registers_struct
	{
		char	*text;
		int		dir_tok_type;
		int		indir_tok_type;
	} registers[] =
	{
		"A",	REG_A,	INDIR_A,
		"B",	REG_B,	INDIR_B,
		"X",	REG_X,	INDIR_X,
		"Y",	REG_Y,	INDIR_Y,
		NULL,	0,		0			/* End of table */
	};
	int		return_code = 0;
	int		registers_idx;

	/***************************
		Check for quoted text
	****************************/

	if( *token == '\'' && *( token + strlen(token) - 1) == '\''
	  )
	{
		return_code = QUOTE;
		goto exit_identify_token;
	}

	/*************************************************
		Check for an invalid character in the token
	**************************************************/

	for( token_idx = 0; *(token + token_idx); token_idx++ )
	{
		tchar = *(token + token_idx);	/* get character from token */
		if( !	( (tchar >= '0' && tchar <= '9') ||
				  (tchar >= 'A' && tchar <= 'Z') ||
				  tchar == '['					 ||
				  tchar == ']'					 ||
				  tchar == '+'					 ||
				  tchar == '-'					 ||
				  tchar == '_'
				)
		  )
		{
			return_code = UNKNOWN_TYPE;
			goto exit_identify_token;
		}
	}

	/**********************************
		Check for a register operand
	***********************************/

	for( registers_idx = 0 ; registers[registers_idx].text ; registers_idx++ )
	{
		if( !strcmp( registers[registers_idx].text, token ) )
		{
			return_code = registers[registers_idx].dir_tok_type;
			goto exit_identify_token;
		}
	}

	/************************************
		Check for an immediate operand
	*************************************/

	if( isdigit( *token ) || *token == '-' || *token == '+' )
	{
		return_code = IMMED;
		goto exit_identify_token;
	}

	/*******************************
		Check for a direct address
	********************************/

	if( isalpha( *token ) )
	{
		return_code = DIR_ADDR;
		goto exit_identify_token;
	}

	/************************************
		Check for an indirect register
	*************************************/

	if( *token == '[' && *(token+2) == ']')
	{
		for( registers_idx = 0 ; registers[registers_idx].text ; registers_idx++ )
		{
			if( !strncmp( registers[registers_idx].text, token+1, 1 ) )
			{
				return_code = registers[registers_idx].indir_tok_type;
				goto exit_identify_token;
			}
		}
	}

	/***********************************
		Check for an indirect address
	************************************/

	if( *token == '['			&&
		isalpha( *(token+1) )	&&
		*( token + strlen(token) - 1) == ']')
	{
		return_code = INDIR_ADDR;
		goto exit_identify_token;
	}
	return_code = UNKNOWN_TYPE;

exit_identify_token:
	/** printf( "token = %s   tok_type = %d\n", token, return_code); **/
	return return_code;
}

/*****************************************************************************
*                                                                            *
*           initialize_data_structures()                                     *
*                                                                            *
******************************************************************************
*
* AUTHOR:	Harold Almon
*
* FUNCTION:	This procedure opens all the files and initializes all
*			the data structures necessary to run the SPASM assembler.
*
* INPUTS:   argc	-> the number of command line arguements
*			argv	-> an array of pointers to the command line args.
*
* OUTPUTS:	Opens the source input file.
*			Displays welcome message.
*			Initializes various data structures.
*
* RETURNS:  0	->	No errors.
*			!0	->	Error occurred.
*/
int initialize_data_structures( int argc, char *argv[], FILE **in_file )
{
	int				error_code = 0;
	static	char	in_file_name[8+1+3+1];
	static	char	out_file_name[8+1+3+1];
	int				symbol_idx;

	/**********************************
		First, open the input source
		code file
	***********************************/

	if( argv[1] != NULL )					/* if name was entered */
	{										/* then copy it to file_name */
		strcpy( in_file_name, argv[1] );
	}
	else									/* if name was NOT entered */
	{
		printf( "Enter file name: ");		/* ask the user */
		scanf("%s", in_file_name);			/* for a file name */
	}

	/**********************************
		Create a valid .asm file name
	***********************************/

	if( !strchr( in_file_name, (int) '.' ) )/* search for decimal point */
	{							  			/* if none found then */
		strcat( in_file_name, ".asm");		/* append .asm to file name */
	}
	printf( "\nSPASM Assembler 1.0, Copyright Harold Almon\n\n");
	printf( "Assembling %s\n", in_file_name );

	/*********************************
		Open source code input file
	**********************************/

	if( (*in_file = fopen ( in_file_name, "r" )) == NULL )      /* open input file */
	{
		error_code = -1;
		fprintf( stderr, "\nasm: ERROR: Unable to open %s\n\n", in_file_name);
	}
	else
	{
		/*****************************
			Create a .lst file name
		******************************/

		strcpy( out_file_name, in_file_name );
		*strchr( out_file_name, (int) '.' ) = '\0';	/* replace '.' with '\0' */
		strcat( out_file_name, ".lst");				/* append .lst to file name */

		/*************************************
			Open source listing output file
		**************************************/

		if( (out_file = fopen ( out_file_name, "w" )) == NULL )
		{
			error_code = -1;
			fprintf( stderr, "\nasm: ERROR: Unable to open %s\n\n", out_file_name);
		}
		fputs("\n\n                     --- SPASM 1.0, Copyright Harold Almon ---\n\n",
			out_file);
		fputs("              LOCATION       LABEL     OPERATOR  OPER 1    OPER 2\n\n",
			out_file);
	}
	/**************************
		Initialize variables
	***************************/

	for( symbol_idx = 0; symbol_idx < SYMBOLS_NUM; symbol_idx++)
	{
		symbol_table[ symbol_idx ].name[0] = '\0';	/* clear entry */
	}
	end_flag = FALSE;
	src_line_ctr = 1;
	location_ctr = 0;
	return 0;
}

/*****************************************************************************
*                                                                            *
*           output_symbols()                                                 *
*                                                                            *
******************************************************************************
*
* AUTHOR:	Harold Almon
*
* FUNCTION:	This procedure produces a symbol table.
*
* INPUTS:	None.
*
* OUTPUTS:	Symbol table is added to the ".lst" file.
*
* RETURNS:  None.
*/
void output_symbols(void)
{
	char	location_ascii[5];
	char	symbol_buf[81];					/* symbol temp buffer */
	int		symbol_idx;

	sprintf(symbol_buf, "\n\n%10s%10s%5s%10s%5s%10s\n\n",
				"",
				"Symbol",
				"",
				"Value",
				"",
				"Src Line");
	fputs( symbol_buf, out_file);			/* print the heading */
	for( symbol_idx = 0; symbol_idx < SYMBOLS_NUM; symbol_idx++)
	{
		if( symbol_table[ symbol_idx ].name[0] != '\0' )
		{
			/********************************
				Convert the symbol's value
				to an upper case string
			*********************************/

			sprintf(location_ascii, "%04x", symbol_table[ symbol_idx ].value);
			sprintf(symbol_buf, "%10s%10.8s%5s%10s%5s%10d\n",	/* print the symbol */
				"",
				symbol_table[ symbol_idx ].name,
				"",
				strupr( location_ascii ),
				"",
				symbol_table[ symbol_idx ].line);
			fputs( symbol_buf, out_file );
		}
	}
}

/*****************************************************************************
*                                                                            *
*           perform_pass_one()                                               *
*                                                                            *
******************************************************************************
*
* AUTHOR:	Harold Almon
*
* FUNCTION:	This procedure performs pass one of the assembler which
*			includes parsing the input into tokens and validating
*			the code syntax.
*
* INPUTS:	in_file		->	pointer to input file structure
*
* OUTPUTS:	Creates a listing file in <file_spec>.lst.
*
* RETURNS:	0.
*/
int	perform_pass_one( FILE *in_file )
{
	#define BUF_SIZE 200

	int		error_code = 0;
	char	in_buf[BUF_SIZE];           /* input buffer for source code */
	int		instr_match;				/* the valid code template */
	struct	token_info_struct (* line_tokens)[MAX_TOKENS];
	while( fgets( in_buf, BUF_SIZE, in_file ) != NULL )
	{
		if( !(line_tokens = parse_line( in_buf )) )
			break;
		if( instr_match = check_syntax( line_tokens ) )
			generate_code( instr_match, line_tokens );
	}
	if( ferror( in_file ) )
	{
		error_code = 1;
		fprintf( stderr, "*** SPASM: Error: File read error\n" );
		goto exit_perform_pass_one;
	}
	if( in_file )
	{
		if( !end_flag )
		{
			fputs("*** SPASM: ERROR: Missing END statement\n", out_file);
		}
		fclose( in_file );			/* close input source file */
	}
	output_symbols();				/* output the symbol table */

exit_perform_pass_one:
	return error_code;
}

/*****************************************************************************
*                                                                            *
*           parse_line()                                                     *
*                                                                            *
******************************************************************************
*
* AUTHOR:	Harold Almon
*
* FUNCTION:	This procedure takes one line of source code and separates
*			the line into individual tokens which are stored in an
*			array of structures.
*
* INPUTS:	in_buf	->	a pointer to the line to be parsed.
*
* OUTPUTS:	None.
*
* RETURNS:  A pointer to an array of token_info_struct which contains
*			the tokens.
*/
struct	token_info_struct (* parse_line ( char *in_buf ) )[MAX_TOKENS]
{
	char	*buf_ptr;			/* pointer to the source code buffer */
	int		column;				/* current column no. in the source code */
	int		error_code = 0;
	int		token_ctr;			/* counts the number of tokens per line */
	struct	token_info_struct *token_ptr;
	static	struct token_info_struct	line_tokens[ MAX_TOKENS ];

	/****************************************
		Initialize the line tokens so that
		they all contain null pointers
	*****************************************/

	for( token_ctr = 0; token_ctr < MAX_TOKENS; token_ctr++ )
	{
		line_tokens[ token_ctr ].token = NULL;	/* reset token ptr null */
	}
	buf_ptr = in_buf;
	column = 1;
	token_ctr = 0;
	while( (token_ptr = get_token( buf_ptr, column ) ) != NULL
			&& token_ctr < MAX_TOKENS )
	{
		/* printf( "\ttoken = %s\n", token_ptr -> token ); */

		/*******************************************
			Add the length of the token plus the
			number of white spaces skipped in the
			source buffer to the column position
		********************************************/

		buf_ptr += token_ptr -> length + token_ptr -> position - column;
		column = token_ptr -> position;
		line_tokens[ token_ctr++ ] = *token_ptr; /* copy structure */
	}
	return (&line_tokens);		/* return a pointer to the token array */
}

/*****************************************************************************
*                                                                            *
*           pseudo_op()                                                      *
*                                                                            *
******************************************************************************
*
* AUTHOR:	Harold Almon
*
* FUNCTION:	This procedure performs the required operations for the
*			following psuedo operations:
*
*				ORG, EQU, DB, DS, END
*
* INPUTS:	instruction	-> pointer to the pseudo operation
*			op_code_idx	-> the token no. of the operator in the line
*			line_tokens	-> structure containing the tokens in this line
*
* OUTPUTS:	Depends on psuedo op:
*
*				ORG		->	pc <- operand 1
*				EQU		->	symbol added to symbol table
*				DB		->	pc incremented based on operands
*				DS		->	pc <- pc + value of operand 1
*				END		->	sets the end_flag for syntax checking
*
* RETURNS:	None.
*/
void pseudo_op( char *instruction, int op_code_idx,
					struct token_info_struct (* line_tokens)[MAX_TOKENS] )
{
	char	*token;			/* the current token */
	int		token_idx;		/* token index in line_tokens */
	int		token_type;		/* stores the type of the current token */
	if( end_flag )
	{
		fputs( "*** SPASM: ERROR: End of file Expected\n", out_file );
		end_flag = FALSE;		/* prevents re-display of this message */
		goto exit_pseudo_op;
	}

	token = (* line_tokens)[op_code_idx+1].token;
	if( !strcmp( instruction, "ORG" ) )
	{
		location_ctr = asc_to_int( token );
	} else

	if( !strcmp( instruction, "DS" ) )
	{
		location_ctr += asc_to_int( token );
	} else

	if( !strcmp( instruction, "END" ) )
	{
		end_flag = TRUE;
	} else

	if( !strcmp( instruction, "DB" ) )
	{
		/**********************************************
			Identify the line tokens and determine
			how much space is necessary to hold them
		***********************************************/

		for( token_idx = 1;
			(* line_tokens)[op_code_idx+token_idx].token != NULL  ;
			token_idx++ )
		{
			token = (* line_tokens)[op_code_idx + token_idx].token;
			token_type = (* line_tokens)[op_code_idx + token_idx].token_type;
			switch( token_type )
			{
				case QUOTE:
					location_ctr += strlen(
						(* line_tokens)[op_code_idx + token_idx].token) - 2;
					break;

				case IMMED:
				/* also matches case DIR_ADDR: */
					location_ctr += 1;
					break;

				default:
					fputs( "*** SPASM: ERROR: Illegal operand in statement\n", out_file);
			}
			token_idx++;		/* examine the next token */

			/*****************************************
				The next token should be either a
				comma or the end of the line (NULL)
			******************************************/

			token = (* line_tokens)[op_code_idx + token_idx].token;
			if( *token != ',' && token != NULL )
			{
				fputs("*** SPASM: ERROR: Missing comma in expression\n", out_file);
				goto exit_pseudo_op;
			}
		}
		if( token != NULL )
		{
			fputs("*** SPASM: ERROR: Missing expression after comma\n", out_file);
			goto exit_pseudo_op;
		}
	}


exit_pseudo_op:
	return;
}

/*****************************************************************************
*                                                                            *
*           store_symbol()                                                   *
*                                                                            *
******************************************************************************
*
* AUTHOR:	Harold Almon
*
* FUNCTION:	This procedure stores a symbol along with its value in the
*			symbol table.
*
* INPUTS:	symbol			-> pointer to the symbol (ASCIIZ)
*			symbol_value	-> the location of the symbol in the object code
*			source_line		-> the source line number of the symbol
*
* OUTPUTS:  Symbol is stored in the symbol table.
*
* RETURNS:	0 if symbol inserted
*			-1 if error or table full
*/
int store_symbol( char *symbol, int symbol_value, int source_line )
{
	int     error_code  = 0;
	int     return_code = 0;
	int     symbol_idx;
	symbol_idx = hash_name( symbol );	/* get the hash value of symbol */
	/*
		This code seraches through the symbol table
		until an empty slot or a duplicate name
		is found.  The symbol is then inserted into
		the symbol table or an error code is returned.
	*/
	while( TRUE )
	{
		/***********************************
			if slot is empty then store
			the symbol in the symbol table
		************************************/

		if( symbol_table[ symbol_idx ].name[0] == '\0' )
		{
			strcpy( symbol_table[ symbol_idx ].name, symbol );
			symbol_table[ symbol_idx ].value = symbol_value;
			symbol_table[ symbol_idx ].line = source_line;
			break;
		}
		else		/* slot is occupied */
		{
			/******************************
				Check to see if this name
				is already in the table
			*******************************/

			if( !strcmp( symbol_table[ symbol_idx].name, symbol ) )
			{
				return_code = 1;	/* indicate name already in table */
				goto exit_store_symbol;
			}
			symbol_idx = (symbol_idx + 1) % 100;	/* try next position */
		}
	}
exit_store_symbol:
	return error_code;
}
