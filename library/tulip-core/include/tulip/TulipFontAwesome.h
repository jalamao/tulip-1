/*
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */

#ifndef TULIPFONTAWESOME_H
#define TULIPFONTAWESOME_H

#include <tulip/tulipconf.h>

#include <vector>

namespace tlp {

/**
 * @brief Helper class for the configuration of a Font Awesome glyph.
 *
 * Font Awesome is a free iconic font by Dave Gandy (see http://fontawesome.io)
 * offering more than 500 customizable scalable vector icons.
 *
 * A glyph has been added to Tulip enabling to use these great icons
 * as nodes and edges extremities shapes.
 *
 * That class offers utility functions and supported icons names constants.
 *
 * To set a node glyph as a Font Awesome icon, the Font Awesome glyph must be associated
 * to the node through the modification of the "viewShape" integer property attached to the graph.
 * The name of the icon to use must then be set in the "viewFontAwesomeIcon" string property.
 * As an example, the following code snippet activates the Font Awesome glyph for all nodes
 * and sets the "user" icon.
 *
 * @code
 *    // graph is a pointer to a tlp::Graph object
 *    tlp::IntegerProperty *viewShape = graph->getProperty<tlp::IntegerProperty>("viewShape");
 *    tlp::StringProperty *viewFontAwesomeIcon = graph->getProperty<tlp::StringProperty>("viewFontAwesomeIcon");
 *
 *    // sets the Font Awesome glyph on all nodes
 *    viewShape->setAllNodeValue(tlp::NodeShape::FontAwesomeIcon);
 *    // sets the "user" glyph for all nodes
 *    viewFontAwesomeIcon->setAllNodeValue(tlp::TulipFontAwesome::User);
 * @endcode
 **/

class TLP_SCOPE TulipFontAwesome {

public:

  /**
   * Returns the location of the Font Awesome ttf file bundled with Tulip
   */
  static std::string getFontAwesomeTrueTypeFileLocation();

  /**
   * Returns the list of supported Font Awesome icons names
   */
  static const std::vector<std::string> &getSupportedFontAwesomeIcons();

  /**
   * Checks if the provided Font Awesome icon name is supported
   * @param iconName the name of the icon to check support
   */
  static bool isFontAwesomeIconSupported(const std::string& iconName);

  /**
   * Returns the Unicode code point associated to an icon name
   * @param iconName the name of the icon to get the codepoint
   */
  static unsigned int getFontAwesomeIconCodePoint(const std::string& iconName);

  /**
   * Returns an UTF-8 encoded string of a Font Awesome icon
   * @param iconName a Font Awesome icon name
   * @return
   */
  static std::string getFontAwesomeIconUtf8String(const std::string& iconName);

  // static constants for the Font Awesome 4.3 icons names
  static const char* Adjust;
  static const char* Adn;
  static const char* AlignCenter;
  static const char* AlignJustify;
  static const char* AlignLeft;
  static const char* AlignRight;
  static const char* Ambulance;
  static const char* Anchor;
  static const char* Android;
  static const char* Angellist;
  static const char* AngleDoubleDown;
  static const char* AngleDoubleLeft;
  static const char* AngleDoubleRight;
  static const char* AngleDoubleUp;
  static const char* AngleDown;
  static const char* AngleLeft;
  static const char* AngleRight;
  static const char* AngleUp;
  static const char* Apple;
  static const char* Archive;
  static const char* AreaChart;
  static const char* ArrowCircleDown;
  static const char* ArrowCircleLeft;
  static const char* ArrowCircleODown;
  static const char* ArrowCircleOLeft;
  static const char* ArrowCircleORight;
  static const char* ArrowCircleOUp;
  static const char* ArrowCircleRight;
  static const char* ArrowCircleUp;
  static const char* ArrowDown;
  static const char* ArrowLeft;
  static const char* ArrowRight;
  static const char* ArrowUp;
  static const char* Arrows;
  static const char* ArrowsAlt;
  static const char* ArrowsH;
  static const char* ArrowsV;
  static const char* Asterisk;
  static const char* At;
  static const char* Automobile;
  static const char* Backward;
  static const char* Ban;
  static const char* Bank;
  static const char* BarChart;
  static const char* BarChartO;
  static const char* Barcode;
  static const char* Bars;
  static const char* Bed;
  static const char* Beer;
  static const char* Behance;
  static const char* BehanceSquare;
  static const char* Bell;
  static const char* BellO;
  static const char* BellSlash;
  static const char* BellSlashO;
  static const char* Bicycle;
  static const char* Binoculars;
  static const char* BirthdayCake;
  static const char* Bitbucket;
  static const char* BitbucketSquare;
  static const char* Bitcoin;
  static const char* Bold;
  static const char* Bolt;
  static const char* Bomb;
  static const char* Book;
  static const char* Bookmark;
  static const char* BookmarkO;
  static const char* Briefcase;
  static const char* Btc;
  static const char* Bug;
  static const char* Building;
  static const char* BuildingO;
  static const char* Bullhorn;
  static const char* Bullseye;
  static const char* Bus;
  static const char* Buysellads;
  static const char* Cab;
  static const char* Calculator;
  static const char* Calendar;
  static const char* CalendarO;
  static const char* Camera;
  static const char* CameraRetro;
  static const char* Car;
  static const char* CaretDown;
  static const char* CaretLeft;
  static const char* CaretRight;
  static const char* CaretSquareODown;
  static const char* CaretSquareOLeft;
  static const char* CaretSquareORight;
  static const char* CaretSquareOUp;
  static const char* CaretUp;
  static const char* CartArrowDown;
  static const char* CartPlus;
  static const char* Cc;
  static const char* CcAmex;
  static const char* CcDiscover;
  static const char* CcMastercard;
  static const char* CcPaypal;
  static const char* CcStripe;
  static const char* CcVisa;
  static const char* Certificate;
  static const char* Chain;
  static const char* ChainBroken;
  static const char* Check;
  static const char* CheckCircle;
  static const char* CheckCircleO;
  static const char* CheckSquare;
  static const char* CheckSquareO;
  static const char* ChevronCircleDown;
  static const char* ChevronCircleLeft;
  static const char* ChevronCircleRight;
  static const char* ChevronCircleUp;
  static const char* ChevronDown;
  static const char* ChevronLeft;
  static const char* ChevronRight;
  static const char* ChevronUp;
  static const char* Child;
  static const char* Circle;
  static const char* CircleO;
  static const char* CircleONotch;
  static const char* CircleThin;
  static const char* Clipboard;
  static const char* ClockO;
  static const char* Close;
  static const char* Cloud;
  static const char* CloudDownload;
  static const char* CloudUpload;
  static const char* Cny;
  static const char* Code;
  static const char* CodeFork;
  static const char* Codepen;
  static const char* Coffee;
  static const char* Cog;
  static const char* Cogs;
  static const char* Columns;
  static const char* Comment;
  static const char* CommentO;
  static const char* Comments;
  static const char* CommentsO;
  static const char* Compass;
  static const char* Compress;
  static const char* Connectdevelop;
  static const char* Copy;
  static const char* Copyright;
  static const char* CreditCard;
  static const char* Crop;
  static const char* Crosshairs;
  static const char* Css3;
  static const char* Cube;
  static const char* Cubes;
  static const char* Cut;
  static const char* Cutlery;
  static const char* Dashboard;
  static const char* Dashcube;
  static const char* Database;
  static const char* Dedent;
  static const char* Delicious;
  static const char* Desktop;
  static const char* Deviantart;
  static const char* Diamond;
  static const char* Digg;
  static const char* Dollar;
  static const char* DotCircleO;
  static const char* Download;
  static const char* Dribbble;
  static const char* Dropbox;
  static const char* Drupal;
  static const char* Edit;
  static const char* Eject;
  static const char* EllipsisH;
  static const char* EllipsisV;
  static const char* Empire;
  static const char* Envelope;
  static const char* EnvelopeO;
  static const char* EnvelopeSquare;
  static const char* Eraser;
  static const char* Eur;
  static const char* Euro;
  static const char* Exchange;
  static const char* Exclamation;
  static const char* ExclamationCircle;
  static const char* ExclamationTriangle;
  static const char* Expand;
  static const char* ExternalLink;
  static const char* ExternalLinkSquare;
  static const char* Eye;
  static const char* EyeSlash;
  static const char* Eyedropper;
  static const char* Facebook;
  static const char* FacebookF;
  static const char* FacebookOfficial;
  static const char* FacebookSquare;
  static const char* FastBackward;
  static const char* FastForward;
  static const char* Fax;
  static const char* Female;
  static const char* FighterJet;
  static const char* File;
  static const char* FileArchiveO;
  static const char* FileAudioO;
  static const char* FileCodeO;
  static const char* FileExcelO;
  static const char* FileImageO;
  static const char* FileMovieO;
  static const char* FileO;
  static const char* FilePdfO;
  static const char* FilePhotoO;
  static const char* FilePictureO;
  static const char* FilePowerpointO;
  static const char* FileSoundO;
  static const char* FileText;
  static const char* FileTextO;
  static const char* FileVideoO;
  static const char* FileWordO;
  static const char* FileZipO;
  static const char* FilesO;
  static const char* Film;
  static const char* Filter;
  static const char* Fire;
  static const char* FireExtinguisher;
  static const char* Flag;
  static const char* FlagCheckered;
  static const char* FlagO;
  static const char* Flash;
  static const char* Flask;
  static const char* Flickr;
  static const char* FloppyO;
  static const char* Folder;
  static const char* FolderO;
  static const char* FolderOpen;
  static const char* FolderOpenO;
  static const char* Font;
  static const char* Forumbee;
  static const char* Forward;
  static const char* Foursquare;
  static const char* FrownO;
  static const char* FutbolO;
  static const char* Gamepad;
  static const char* Gavel;
  static const char* Gbp;
  static const char* Ge;
  static const char* Gear;
  static const char* Gears;
  static const char* Genderless;
  static const char* Gift;
  static const char* Git;
  static const char* GitSquare;
  static const char* Github;
  static const char* GithubAlt;
  static const char* GithubSquare;
  static const char* Gittip;
  static const char* Glass;
  static const char* Globe;
  static const char* Google;
  static const char* GooglePlus;
  static const char* GooglePlusSquare;
  static const char* GoogleWallet;
  static const char* GraduationCap;
  static const char* Gratipay;
  static const char* Group;
  static const char* HSquare;
  static const char* HackerNews;
  static const char* HandODown;
  static const char* HandOLeft;
  static const char* HandORight;
  static const char* HandOUp;
  static const char* HddO;
  static const char* Header;
  static const char* Headphones;
  static const char* Heart;
  static const char* HeartO;
  static const char* Heartbeat;
  static const char* History;
  static const char* Home;
  static const char* HospitalO;
  static const char* Hotel;
  static const char* Html5;
  static const char* Ils;
  static const char* Image;
  static const char* Inbox;
  static const char* Indent;
  static const char* Info;
  static const char* InfoCircle;
  static const char* Inr;
  static const char* Instagram;
  static const char* Institution;
  static const char* Ioxhost;
  static const char* Italic;
  static const char* Joomla;
  static const char* Jpy;
  static const char* Jsfiddle;
  static const char* Key;
  static const char* KeyboardO;
  static const char* Krw;
  static const char* Language;
  static const char* Laptop;
  static const char* Lastfm;
  static const char* LastfmSquare;
  static const char* Leaf;
  static const char* Leanpub;
  static const char* Legal;
  static const char* LemonO;
  static const char* LevelDown;
  static const char* LevelUp;
  static const char* LifeBouy;
  static const char* LifeBuoy;
  static const char* LifeRing;
  static const char* LifeSaver;
  static const char* LightbulbO;
  static const char* LineChart;
  static const char* Link;
  static const char* Linkedin;
  static const char* LinkedinSquare;
  static const char* Linux;
  static const char* List;
  static const char* ListAlt;
  static const char* ListOl;
  static const char* ListUl;
  static const char* LocationArrow;
  static const char* Lock;
  static const char* LongArrowDown;
  static const char* LongArrowLeft;
  static const char* LongArrowRight;
  static const char* LongArrowUp;
  static const char* Magic;
  static const char* Magnet;
  static const char* MailForward;
  static const char* MailReply;
  static const char* MailReplyAll;
  static const char* Male;
  static const char* MapMarker;
  static const char* Mars;
  static const char* MarsDouble;
  static const char* MarsStroke;
  static const char* MarsStrokeH;
  static const char* MarsStrokeV;
  static const char* Maxcdn;
  static const char* Meanpath;
  static const char* Medium;
  static const char* Medkit;
  static const char* MehO;
  static const char* Mercury;
  static const char* Microphone;
  static const char* MicrophoneSlash;
  static const char* Minus;
  static const char* MinusCircle;
  static const char* MinusSquare;
  static const char* MinusSquareO;
  static const char* Mobile;
  static const char* MobilePhone;
  static const char* Money;
  static const char* MoonO;
  static const char* MortarBoard;
  static const char* Motorcycle;
  static const char* Music;
  static const char* Navicon;
  static const char* Neuter;
  static const char* NewspaperO;
  static const char* Openid;
  static const char* Outdent;
  static const char* Pagelines;
  static const char* PaintBrush;
  static const char* PaperPlane;
  static const char* PaperPlaneO;
  static const char* Paperclip;
  static const char* Paragraph;
  static const char* Paste;
  static const char* Pause;
  static const char* Paw;
  static const char* Paypal;
  static const char* Pencil;
  static const char* PencilSquare;
  static const char* PencilSquareO;
  static const char* Phone;
  static const char* PhoneSquare;
  static const char* Photo;
  static const char* PictureO;
  static const char* PieChart;
  static const char* PiedPiper;
  static const char* PiedPiperAlt;
  static const char* Pinterest;
  static const char* PinterestP;
  static const char* PinterestSquare;
  static const char* Plane;
  static const char* Play;
  static const char* PlayCircle;
  static const char* PlayCircleO;
  static const char* Plug;
  static const char* Plus;
  static const char* PlusCircle;
  static const char* PlusSquare;
  static const char* PlusSquareO;
  static const char* PowerOff;
  static const char* Print;
  static const char* PuzzlePiece;
  static const char* Qq;
  static const char* Qrcode;
  static const char* Question;
  static const char* QuestionCircle;
  static const char* QuoteLeft;
  static const char* QuoteRight;
  static const char* Ra;
  static const char* Random;
  static const char* Rebel;
  static const char* Recycle;
  static const char* Reddit;
  static const char* RedditSquare;
  static const char* Refresh;
  static const char* Remove;
  static const char* Renren;
  static const char* Reorder;
  static const char* Repeat;
  static const char* Reply;
  static const char* ReplyAll;
  static const char* Retweet;
  static const char* Rmb;
  static const char* Road;
  static const char* Rocket;
  static const char* RotateLeft;
  static const char* RotateRight;
  static const char* Rouble;
  static const char* Rss;
  static const char* RssSquare;
  static const char* Rub;
  static const char* Ruble;
  static const char* Rupee;
  static const char* Save;
  static const char* Scissors;
  static const char* Search;
  static const char* SearchMinus;
  static const char* SearchPlus;
  static const char* Sellsy;
  static const char* Send;
  static const char* SendO;
  static const char* Server;
  static const char* Share;
  static const char* ShareAlt;
  static const char* ShareAltSquare;
  static const char* ShareSquare;
  static const char* ShareSquareO;
  static const char* Shekel;
  static const char* Sheqel;
  static const char* Shield;
  static const char* Ship;
  static const char* Shirtsinbulk;
  static const char* ShoppingCart;
  static const char* SignIn;
  static const char* SignOut;
  static const char* Signal;
  static const char* Simplybuilt;
  static const char* Sitemap;
  static const char* Skyatlas;
  static const char* Skype;
  static const char* Slack;
  static const char* Sliders;
  static const char* Slideshare;
  static const char* SmileO;
  static const char* SoccerBallO;
  static const char* Sort;
  static const char* SortAlphaAsc;
  static const char* SortAlphaDesc;
  static const char* SortAmountAsc;
  static const char* SortAmountDesc;
  static const char* SortAsc;
  static const char* SortDesc;
  static const char* SortDown;
  static const char* SortNumericAsc;
  static const char* SortNumericDesc;
  static const char* SortUp;
  static const char* Soundcloud;
  static const char* SpaceShuttle;
  static const char* Spinner;
  static const char* Spoon;
  static const char* Spotify;
  static const char* Square;
  static const char* SquareO;
  static const char* StackExchange;
  static const char* StackOverflow;
  static const char* Star;
  static const char* StarHalf;
  static const char* StarHalfEmpty;
  static const char* StarHalfFull;
  static const char* StarHalfO;
  static const char* StarO;
  static const char* Steam;
  static const char* SteamSquare;
  static const char* StepBackward;
  static const char* StepForward;
  static const char* Stethoscope;
  static const char* Stop;
  static const char* StreetView;
  static const char* Strikethrough;
  static const char* Stumbleupon;
  static const char* StumbleuponCircle;
  static const char* Subscript;
  static const char* Subway;
  static const char* Suitcase;
  static const char* SunO;
  static const char* Superscript;
  static const char* Support;
  static const char* Table;
  static const char* Tablet;
  static const char* Tachometer;
  static const char* Tag;
  static const char* Tags;
  static const char* Tasks;
  static const char* Taxi;
  static const char* TencentWeibo;
  static const char* Terminal;
  static const char* TextHeight;
  static const char* TextWidth;
  static const char* Th;
  static const char* ThLarge;
  static const char* ThList;
  static const char* ThumbTack;
  static const char* ThumbsDown;
  static const char* ThumbsODown;
  static const char* ThumbsOUp;
  static const char* ThumbsUp;
  static const char* Ticket;
  static const char* Times;
  static const char* TimesCircle;
  static const char* TimesCircleO;
  static const char* Tint;
  static const char* ToggleDown;
  static const char* ToggleLeft;
  static const char* ToggleOff;
  static const char* ToggleOn;
  static const char* ToggleRight;
  static const char* ToggleUp;
  static const char* Train;
  static const char* Transgender;
  static const char* TransgenderAlt;
  static const char* Trash;
  static const char* TrashO;
  static const char* Tree;
  static const char* Trello;
  static const char* Trophy;
  static const char* Truck;
  static const char* Try;
  static const char* Tty;
  static const char* Tumblr;
  static const char* TumblrSquare;
  static const char* TurkishLira;
  static const char* Twitch;
  static const char* Twitter;
  static const char* TwitterSquare;
  static const char* Umbrella;
  static const char* Underline;
  static const char* Undo;
  static const char* University;
  static const char* Unlink;
  static const char* Unlock;
  static const char* UnlockAlt;
  static const char* Unsorted;
  static const char* Upload;
  static const char* Usd;
  static const char* User;
  static const char* UserMd;
  static const char* UserPlus;
  static const char* UserSecret;
  static const char* UserTimes;
  static const char* Users;
  static const char* Venus;
  static const char* VenusDouble;
  static const char* VenusMars;
  static const char* Viacoin;
  static const char* VideoCamera;
  static const char* VimeoSquare;
  static const char* Vine;
  static const char* Vk;
  static const char* VolumeDown;
  static const char* VolumeOff;
  static const char* VolumeUp;
  static const char* Warning;
  static const char* Wechat;
  static const char* Weibo;
  static const char* Weixin;
  static const char* Whatsapp;
  static const char* Wheelchair;
  static const char* Wifi;
  static const char* Windows;
  static const char* Won;
  static const char* Wordpress;
  static const char* Wrench;
  static const char* Xing;
  static const char* XingSquare;
  static const char* Yahoo;
  static const char* Yelp;
  static const char* Yen;
  static const char* Youtube;
  static const char* YoutubePlay;
  static const char* YoutubeSquare;

  // static constants for the new icons added in Font Awesome 4.4
  static const char* Yc;
  static const char* YCombinator;
  static const char* OptinMonster;
  static const char* Opencart;
  static const char* Expeditedssl;
  static const char* Battery4;
  static const char* BatteryFull;
  static const char* Battery3;
  static const char* BatteryThreeQuarters;
  static const char* Battery2;
  static const char* BatteryHalf;
  static const char* Battery1;
  static const char* BatteryQuarter;
  static const char* Battery0;
  static const char* BatteryEmpty;
  static const char* MousePointer;
  static const char* ICursor;
  static const char* ObjectGroup;
  static const char* ObjectUngroup;
  static const char* StickyNote;
  static const char* StickyNoteO;
  static const char* CcJcb;
  static const char* CcDinersClub;
  static const char* Clone;
  static const char* BalanceScale;
  static const char* HourglassO;
  static const char* Hourglass1;
  static const char* HourglassStart;
  static const char* Hourglass2;
  static const char* HourglassHalf;
  static const char* Hourglass3;
  static const char* HourglassEnd;
  static const char* Hourglass;
  static const char* HandGrabO;
  static const char* HandRockO;
  static const char* HandStopO;
  static const char* HandPaperO;
  static const char* HandScissorsO;
  static const char* HandLizardO;
  static const char* HandSpockO;
  static const char* HandPointerO;
  static const char* HandPeaceO;
  static const char* Trademark;
  static const char* Registered;
  static const char* CreativeCommons;
  static const char* Gg;
  static const char* GgCircle;
  static const char* Tripadvisor;
  static const char* Odnoklassniki;
  static const char* OdnoklassnikiSquare;
  static const char* GetPocket;
  static const char* WikipediaW;
  static const char* Safari;
  static const char* Chrome;
  static const char* Firefox;
  static const char* Opera;
  static const char* InternetExplorer;
  static const char* Tv;
  static const char* Television;
  static const char* Contao;
  static const char* Px500;
  static const char* Amazon;
  static const char* CalendarPlusO;
  static const char* CalendarMinusO;
  static const char* CalendarTimesO;
  static const char* CalendarCheckO;
  static const char* Industry;
  static const char* MapPin;
  static const char* MapSigns;
  static const char* MapO;
  static const char* Map;
  static const char* Commenting;
  static const char* CommentingO;
  static const char* Houzz;
  static const char* Vimeo;
  static const char* BlackTie;
  static const char* Fonticons;

  // static constants for the new icons added in Font Awesome 4.5
  static const char * Bluetooth;
  static const char * BluetoothB;
  static const char * Codiepie;
  static const char * CreditCardAlt;
  static const char * Edge;
  static const char * FortAwesome;
  static const char * Hashtag;
  static const char * Mixcloud;
  static const char * Modx;
  static const char * PauseCircle;
  static const char * PauseCircleO;
  static const char * Percent;
  static const char * ProductHunt;
  static const char * RedditAlien;
  static const char * Scribd;
  static const char * ShoppingBag;
  static const char * ShoppingBasket;
  static const char * StopCircle;
  static const char * StopCircleO;
  static const char * Usb;



};

}

#endif // TULIPFONTAWESOME_H

