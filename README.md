iqInfoDoc
===========
iqInfoDoc - приложение для просмотра справочной информации системы РАС УВД Синтез Р2.

Установка iqInfoDoc
-------------------
Для установки iqInfoDoc необходимо выполнить следующие действия:
* С рабочего места, на котором установлен iqInfoDoc, скопировать папки */opt/csw* и */sintez/sintez/bin/iqInfoDoc/* в одноименные папки на рабочее место, на которое необходимо установить iqInfoDoc.
* На целевом рабочем месте запустить и закрыть iqInfoDoc командой *LD_LIBRARY_PATH=/opt/csw/lib /sintez/sintez/bin/iqInfoDoc/iqInfoDoc*. После этого будет создан файл */sintez/sintez/.config/itQuasar/iqInfoDoc.conf* с настройками.
* Отредактировать настройки в файле */sintez/sintez/.config/itQuasar/iqInfoDoc.conf*.
* При необходимости добавить iqInfoDoc в автоматический запуск, через startfile. Для этого необходимо отредактировать файл */sintez/sintez/startfile_atd* (для рабочих мест) или */sintez/sintez/startfile_stuk* (для СТУК), добавив в него строки:
~~~~~~~~{bash}    
    #------------iqInfoDoc---------------
    cd /sintez/sintez/bin/iqInfoDoc
    LD_LIBRARY_PATH=/opt/csw/lib $uexec "./iqInfoDoc -style=Plastique > /dev/null 2>&1 &"
~~~~~~~~
* При необходимости отключить автоматический запуск InfoDoc, закомментировать строку `$uexec "java -jar InfoDoc.jar >/dev/null 2>&1 &"` в файле автоматического запуска (см. предыдущий пункт).
* Перезагрузить все процессы на рабочем месте.

Настройка iqInfoDoc
-------------------
Все настройки iqInfoDoc хранятся в файле */sintez/sintez/.config/itQuasar/iqInfoDoc.conf*. Настройки можно редактировать в простом текстовом редакторе. После изменения настроек, необходимо перезапустить iqInfoDoc.

###Описание настроек
* *scaleFactors* - список масштабов, отображаемых в выпадающем меню выбора масштаба. Содержит в себе числа, разделенные запятыми. Пример: `200, 150, 125, 100, 75, 50, 25`.
* *contentUrl* - URL к файлу *content.xml*. Данный файл содержит в себе иерархическую структуру документов iqInfoDoc. Пример `http://web/content.xml`.
* *homePage* - URL к домашней странице. Данная страница открывается при запуске iqInfoDoc, а так же при нажатии на кнопку *Домой*. Пример: `http://web/index_new.htm`.

Обновление iqInfoDoc
--------------------
Для обновления iqInfoDoc необходимо на web-сервере отредактировать файл */sd/web/htdocs/content.xml*. Данный файл содержит иерархическую структуру документов справки. Корнем документа является нода `<content>`. Она может содержать ноды `<folder>` или `<page>`. Нода `<folder>` может так же содержать в себе ноды `<folder>` или `<page>`. 

После редактирования файла, необходимо перезапустить iqInfoDoc на всех рабочих местах, чтоб он заново перечитал файл */sd/web/htdocs/content.xml/*, т.к. данный файл iqInfoDoc загружает с сервера один раз, при запуске приложения.

###Нода `<folder>`
Данная нода отображается в иерархической структуре документа в виде папки. Данная нода может содержать в себе другие ноды `<folder>` или `<page>`.

Нода имеет следующие атрибуты:
* `tilte` - название. Текст данного атрибута в iqInfoDoc будет отображен рядом со значком папки.

###Нода `<page>`
Данная нода отображается в иерархической структуре документа в виде страницы. Данная нода не может содержать в себе другие ноды. В iqInfoDoc, при нажатии на элемент, связанный с данной нодой, происходит открытие страницы, указанной в атрибуте `href` ноды. Текст, указанный внутри ноды, в iqInfoDoc будет отображен рядом со значком странички.

Нода имеет следующие атрибуты:
* `href` - ссылка на страницу. Ссылка может ссылаться либо на html-страницу, либо на pdf-документ. Пути в ссылке должны быть полные, например `http://web/RefAirdrome/wind.htm`.

###Ограничения html-документов
В html-документах не должны присутствовать JavaScript-скрипты, т.к. iqInfoDoc, не обрабатывает их. Так же в документах не рекомендуется делать поля для ввода текста на русском языке, т.к. система РАС УВД Синтез Р2 не позволяет переключиться на русский язык.
