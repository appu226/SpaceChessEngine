#include <gtest/gtest.h>
#include <common/base.h>
#include <chess/board.h>
#include <chess/board_impl.h>
#include <chess/fen.h>
#include <algo_linear/algoLinear.h>
#include <algo_linear/algoGeneric.h>


TEST(BoardSuite, StartingBoardTest) {
	using namespace space;
	IBoard::Ptr startingBoard = BoardImpl::getStartingBoard();
	auto startingFen = Fen::fromBoard(startingBoard, 0, 1);
	std::string expectedStartingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	ASSERT_EQ(startingFen.fen, expectedStartingFen);
	ASSERT_EQ(Fen::fromBoard(BoardImpl::fromFen(startingFen), 0, 1).fen, expectedStartingFen);

}


TEST(BoardSuite, BoardBasicsTest) {
	using namespace space;
	IBoard::Ptr startingBoard = BoardImpl::getStartingBoard();
	auto aa = startingBoard->getPiece({ 0,4 });
	ASSERT_EQ(aa.has_value(), true);
	ASSERT_EQ(aa.value().color, Color::White);
	ASSERT_EQ(aa.value().pieceType, PieceType::King);

	auto ab = startingBoard->getPiece({ 3,4 });
	ASSERT_EQ(ab.has_value(), false);

	auto ac = startingBoard->whoPlaysNext();
	ASSERT_EQ(ac, Color::White);

	bool ad = startingBoard->canCastleLeft(Color::White) &&
				startingBoard->canCastleLeft(Color::Black) &&
				startingBoard->canCastleRight(Color::White) &&
				startingBoard->canCastleRight(Color::Black);
	ASSERT_TRUE(ad);


}

TEST(BoardSuite, BoardUpdateTest) {
	using namespace space;
	IBoard::Ptr startingBoard = BoardImpl::getStartingBoard();

	auto ae = startingBoard->updateBoard({ 1,0,2,0 });
	ASSERT_TRUE(ae.has_value());

	std::string aefen = Fen::fromBoard(ae.value(), 0, 1).fen;
	ASSERT_NO_THROW(ae);
	std::string aefenExpected = "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 1";
	ASSERT_EQ(aefen, aefenExpected);



}

TEST(BoardSuite, BoardMovesTest) {
	using namespace space;
/*	BoardImpl::Ptr startingBoard = BoardImpl::getStartingBoard();
	auto& allMoves = startingBoard->getValidMoves();
	ASSERT_EQ(allMoves.size(), 20);

	auto ba = startingBoard->updateBoard({ 1,0,2,0 });
	ASSERT_TRUE(ba.has_value());

	auto bb = ba.value()->getValidMoves();
	ASSERT_EQ(bb.size(), 20);
*/
/*	std::string foolsMateFEN = "rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR b KQkq - 2 3";
	auto bc = BoardImpl::fromFen(Fen::Fen(foolsMateFEN));
	auto bc2 = bc->isCheckMate();
	ASSERT_TRUE(!bc2);

	auto bc3 = bc->getValidMoves();
	std::vector<Position> moves;
	for (auto it = bc3.begin(); it != bc3.end(); it++) {
		Move m = it->first;
		if (m.sourceRank == 7 && m.sourceFile == 5)
			moves.push_back({ m.destinationRank, m.destinationFile });
	}
	ASSERT_GT(bc3.size(), 0);
*/
	std::string arabianFen = "7k/7R/4KN2/8/8/8/8/8 w - - 2 3";
	auto arabian = BoardImpl::fromFen(arabianFen);
	auto bd = arabian->getValidMoves();
	ASSERT_GT(bd.size(), 0);

	auto bd2 = Fen::moves2string(arabian, bd);

	ASSERT_GT(bd2.size(), 0);
//	Fen::moves2string(arabian, bd);



}


TEST(AlgoSuite, AlgoLinearTest) {
	std::vector<double> wts01 = {1, 5, 4, 4, 10};
	using namespace space;

	//Fen boardfen = Fen("8/8/2kq1r2/8/2KBNR2/8/8/8 b - - 0 0");
	Fen boardfen = Fen("1n1qk1nr/8/8/4NP2/3P4/1pP3Pp/rB5P/3Q1RKB w - - 0 0");
	auto b0 = BoardImpl::fromFen(boardfen);

    //auto aa = AlgoLinearDepthOne(wts01);
	auto aa = AlgoLinearDepthTwoExt(15, wts01);

	Move m0 = aa.getNextMove(b0);
	auto b1 = b0->updateBoard(m0);
	ASSERT_TRUE(b1.has_value());

	Move m1 = aa.getNextMove(b1.value());
	auto b2 = b1.value()->updateBoard(m1);
	ASSERT_TRUE(b2.has_value());
}


TEST(AlgoSuite, AlgoGenericTest) {
	using namespace space;

	Fen boardfen = Fen("1n1qk1nr/8/8/4NP2/3P4/1pP3Pp/rB5P/3Q1RKB w - - 0 0");
	// Fen boardfen = Fen("8/8/q4k2/2n5/8/8/8/bK6 w - - 0 0");

	// BoardImpl::initializeCounter();

	auto b0 = BoardImpl::fromFen(boardfen);
	auto aa = Algo442();

	
	Move m0 = aa.getNextMove(b0);


}
