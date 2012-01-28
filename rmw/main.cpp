#include <QtCore>

int main(int argc, char *argv[])
{
	QTextStream qout(stdout);

	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			QString filename(argv[i]);

			if (
				filename.contains(QRegExp(".*\\.o$")) ||
				filename.contains(QRegExp(".*\\.obj$")) ||
				filename.contains(QRegExp(".*\\.a$")) ||
				filename.contains(QRegExp(".*\\.exe$")) ||
				filename.contains(QRegExp(".*~$"))
			)
			{
				QFile file(filename);
				if (file.exists())
					qout << "removing: " << filename << " result: " << file.remove() << "\n";
			}
		}
		qout << "done\n";
	}
	else
	{
		qout << "no parameters\n";
	}

	qout.flush();
}
