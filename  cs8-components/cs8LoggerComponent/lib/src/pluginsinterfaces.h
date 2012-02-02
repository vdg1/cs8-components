#ifndef __PLUGINSINTERFACES_H__
#define __PLUGINSINTERFACES_H__

class TextEditInterface
{
public:
	enum Action{ReplaceSelection, ReplaceAll, Append};
    virtual ~TextEditInterface() {}
	virtual QString menuName() const = 0;
    virtual QString text(QString text, QString selectedText) = 0;
    virtual TextEditInterface::Action action() const = 0;
    virtual bool hasConfigDialog() const = 0;
    virtual void config() = 0;
};

Q_DECLARE_INTERFACE(TextEditInterface,
                    "qdevelop.TextEditInterface/1.0")

#endif // __PLUGINSINTERFACES_H__
		