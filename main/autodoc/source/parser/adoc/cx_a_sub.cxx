/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/

#include <precomp.h>
#include <adoc/cx_a_sub.hxx>


// NOT FULLY DEFINED SERVICES
#include <tokens/parseinc.hxx>
#include <x_parse.hxx>
#include <adoc/tk_docw.hxx>


namespace adoc {

//************************	  Cx_LineStart		************************//

Cx_LineStart::Cx_LineStart( TkpContext &	i_rFollowUpContext )
	: 	pDealer(0),
		pFollowUpContext(&i_rFollowUpContext)
{
}

void
Cx_LineStart::ReadCharChain( CharacterSource & io_rText )
{
	uintt nCount = 0;
	for ( char cNext = io_rText.CurChar(); cNext == 32 OR cNext == 9; cNext = io_rText.MoveOn() )
	{
		if (cNext == 32)
			nCount++;
		else if (cNext == 9)
			nCount += 4;
	}
	io_rText.CutToken();

	if (nCount < 50)
		pNewToken = new Tok_LineStart(UINT8(nCount));
	else
		pNewToken = new Tok_LineStart(0);
}

bool
Cx_LineStart::PassNewToken()
{
	if (pNewToken)
	{
		pNewToken.Release()->DealOut(*pDealer);
		return true;
	}
	return false;
}

TkpContext &
Cx_LineStart::FollowUpContext()
{
	return *pFollowUpContext;
}


//************************	  Cx_CheckStar		************************//

Cx_CheckStar::Cx_CheckStar(	TkpContext &		i_rFollowUpContext )
	: 	pDealer(0),
		pFollowUpContext(&i_rFollowUpContext),
		pEnd_FollowUpContext(0),
		bCanBeEnd(false),
		bEndTokenFound(false)
{
}


void
Cx_CheckStar::ReadCharChain( CharacterSource & io_rText )
{
	bEndTokenFound = false;
	if (bCanBeEnd)
	{
		char cNext = jumpOver(io_rText,'*');
		if ( NULCH == cNext )
			throw X_Parser(X_Parser::x_UnexpectedEOF, "", String::Null_(), 0);
		if (cNext == '/')
		{
			io_rText.MoveOn();
			pNewToken = new Tok_EoDocu;
			bEndTokenFound = true;
		}
		else
		{
			pNewToken = new Tok_DocWord(io_rText.CutToken());
		}
	}
	else
	{
		jumpToWhite(io_rText);
		pNewToken = new Tok_DocWord(io_rText.CutToken());
	}
}

bool
Cx_CheckStar::PassNewToken()
{
	if (pNewToken)
	{
		pNewToken.Release()->DealOut(*pDealer);
		return true;
	}
	return false;
}

TkpContext &
Cx_CheckStar::FollowUpContext()
{
	if (bEndTokenFound)
		return *pEnd_FollowUpContext;
	else
		return *pFollowUpContext;
}


//************************	  Cx_AtTagCompletion		************************//

Cx_AtTagCompletion::Cx_AtTagCompletion( TkpContext &	i_rFollowUpContext )
	: 	pDealer(0),
		pFollowUpContext(&i_rFollowUpContext)
{
}

void
Cx_AtTagCompletion::ReadCharChain( CharacterSource & io_rText )
{
	jumpToWhite(io_rText);
	csv_assert(fCur_TokenCreateFunction != 0);
	pNewToken = (*fCur_TokenCreateFunction)(io_rText.CutToken());
}

bool
Cx_AtTagCompletion::PassNewToken()
{
	if (pNewToken)
	{
		pNewToken.Release()->DealOut(*pDealer);
		return true;
	}
	return false;
}

TkpContext &
Cx_AtTagCompletion::FollowUpContext()
{
	return *pFollowUpContext;
}




}   // namespace adoc

