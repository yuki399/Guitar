#ifndef FILEDIFFWIDGET_H
#define FILEDIFFWIDGET_H

#include <QDialog>
#include "Git.h"
#include "MainWindow.h"

namespace Ui {
class FileDiffWidget;
}

enum class ViewType {
	None,
	Left,
	Right
};

enum class FilePreviewType {
	None,
	Text,
	Image,
};

struct TextDiffLine {
	enum Type {
		Unknown,
		Normal,
		Add,
		Del,
	} type = Unknown;
	int hunk_number = -1;
	int line_number = -1;
	QString text;
	TextDiffLine()
	{
	}
	TextDiffLine(QString const &text, Type type)
		: type(type)
		, text(text)
	{
	}
};


class QTableWidgetItem;

class FileDiffWidget : public QWidget
{
	Q_OBJECT
	friend class BigDiffWindow;
public:
	struct DiffData {
		struct Content {
			QString id;
			QString path;
			QByteArray bytes;
			QList<TextDiffLine> lines;
		};
		Content left;
		Content right;
		QStringList original_lines;
	};

	struct DrawData {
		int v_scroll_pos = 0;
		int h_scroll_pos = 0;
		int char_width = 0;
		int line_height = 0;
		QColor bgcolor_text;
		QColor bgcolor_add;
		QColor bgcolor_del;
		QColor bgcolor_add_dark;
		QColor bgcolor_del_dark;
		QColor bgcolor_gray;
		QWidget *forcus = nullptr;
		DrawData();
	};

	enum ViewStyle {
		None,
		SingleFile,
		LeftOnly,
		RightOnly,
		SideBySide,
	};

private:
	Ui::FileDiffWidget *ui;
	struct Private;
	Private *pv;

	struct InitParam_ {
		ViewStyle view_style = ViewStyle::None;
		QByteArray bytes;
		Git::Diff diff;
		bool uncommited = false;
		QString workingdir;
	};

	FileDiffWidget::DiffData *diffdata();
	FileDiffWidget::DiffData const *diffdata() const;
	FileDiffWidget::DrawData *drawdata();
	FileDiffWidget::DrawData const *drawdata() const;

	ViewStyle viewstyle() const;

	GitPtr git();
	Git::Object cat_file(GitPtr g, const QString &id);

	int totalTextLines() const
	{
		return diffdata()->left.lines.size();
	}

	int fileviewScrollPos() const
	{
		return drawdata()->v_scroll_pos;
	}

	int visibleLines() const
	{
		int n = 0;
		if (drawdata()->line_height > 0) {
			n = fileviewHeight() / drawdata()->line_height;
			if (n < 1) n = 1;
		}
		return n;
	}

	void scrollTo(int value);

	void resetScrollBarValue();
	void updateVerticalScrollBar();
	void updateHorizontalScrollBar(int headerchars);
	void updateSliderCursor();
	void updateControls();

	int fileviewHeight() const;

	void setDiffText(const QList<TextDiffLine> &left, const QList<TextDiffLine> &right);


	void setLeftOnly(const QByteArray &ba, const Git::Diff &diff);
	void setRightOnly(const QByteArray &ba, const Git::Diff &diff);
	void setSideBySide(const QByteArray &ba, const Git::Diff &diff, bool uncommited, const QString &workingdir);

	bool isValidID_(const QString &id);

	FilePreviewType setupPreviewWidget();

	void makeSideBySideDiffData(QList<TextDiffLine> *left_lines, QList<TextDiffLine> *right_lines) const;
	void setBinaryMode(bool f);
public:
	explicit FileDiffWidget(QWidget *parent = 0);
	~FileDiffWidget();

	void bind(MainWindow *mw);

	void clearDiffView();

	void setSingleFile(QByteArray const &ba, const QString &id, const QString &path);

	void updateDiffView(const Git::Diff &info, bool uncommited);
	void updateDiffView(QString id_left, QString id_right);

	QPixmap makeDiffPixmap(ViewType side, int width, int height, const DiffData *diffdata, const FileDiffWidget::DrawData *drawdata);

	void setMaximizeButtonEnabled(bool f);
	void setLeftBorderVisible(bool f);
private slots:
	void onVerticalScrollValueChanged(int value);
	void onHorizontalScrollValueChanged(int value);
	void onDiffWidgetWheelScroll(int lines);
	void onScrollValueChanged2(int value);
	void onDiffWidgetResized();
	void on_toolButton_fullscreen_clicked();

	void setBinaryMode();
protected:
	bool eventFilter(QObject *watched, QEvent *event);
signals:
	void moveNextItem();
	void movePreviousItem();
};

#endif // FILEDIFFWIDGET_H
