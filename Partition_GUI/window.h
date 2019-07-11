#ifndef WINDOW_H
#define WINDOW_H
#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineEdit>
#include <QTextEdit>

using namespace std;
class QPushButton;
class Window : public QWidget
{
 Q_OBJECT
public:
 explicit Window(QWidget *parent = 0);
private slots:
 void slotButtonClicked(bool);
 void slotEnterButtonClicked(bool);
 void submitButtonClicked(bool clicked);
 void browseButtonClicked();
 void slotFinishButtonClicked();
 //void slotColsEntered();
 //void slotRowsEntered();

private:
 void displayButtons();
 std::vector< std::vector<QPushButton *> > buttons;
 std::vector<std::vector<bool> > in_partition;
 QPushButton *enter_button;
 QPushButton *submit_button;
 QPushButton *browse_button;
 QPushButton *finish_button;
 QPushButton *dir_browse_button;
 QLabel *map;
 QLabel *image_select;
 QLabel *dir_select;
 std::vector<int> clicked_buttons;

 int r;
 int c;
 int num_partitions;
 int num_states_in_partition;
 int current_partition_size;
 std::ofstream *outfile;
 void closeEvent(QCloseEvent *bar);
 void clearButtons();
 QPixmap image;
 QImage  *imageObject;
 QGraphicsScene *scene;
 QGraphicsView *view;
 QLineEdit *enter_rows;
 QLineEdit *enter_cols;
 bool rows_entered;
 bool cols_entered;
 bool is_number(const std::string& s);
};

#endif // WINDOW_H
