#include "BudgetView.h"
#include <QApplication>

int main(int argc, char *argv[]){
	
	QApplication lApp(argc, argv);

	BudgetView *lBudgetView = new BudgetView();
	lBudgetView; // Squash compiler warning
	
	return lApp.exec();
}	// int main(int argc, char *argv[])
