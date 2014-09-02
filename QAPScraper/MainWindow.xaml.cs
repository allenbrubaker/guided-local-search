using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Net;
using System.IO;
using System.Xml.Linq;
using System.Text.RegularExpressions;
namespace QAPLibScraper
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		public MainWindow()
		{
			InitializeComponent();
			tbUrl.Text = "http://www.seas.upenn.edu/qaplib/inst.html#El";
		}

		private void btnScrape_Click(object sender, RoutedEventArgs e)
		{
			tbResult.Text = Scrape(tbUrl.Text.Trim());
		}

		private string Scrape(string url)
		{
			string html = new StreamReader(WebRequest.Create(url).GetResponse().GetResponseStream()).ReadToEnd();
			List<string> names = new List<string>(), opt = new List<string>(), algo = new List<string>();
			Func<string, string> fix = x => x.Trim().ToLower();
			foreach (Match m in Regex.Matches(html, @"<pre>[\s\S]*?</pre>", RegexOptions.IgnoreCase | RegexOptions.Multiline))
			{
				string text = m.Value;
				RegexOptions options = RegexOptions.IgnoreCase | RegexOptions.Multiline;
				text = Regex.Replace(text, @"<.*?>\*</a>", "", options);
				foreach (Match mm in Regex.Matches(text, @"^\s*<a .*?>(.*?)</a>\s*[\d]{2,3}\s*(\d+)\s*\((.*?)\)", options))
				{
					names.Add(fix(mm.Groups[1].Value));
					opt.Add(fix(mm.Groups[2].Value));
					algo.Add(fix(mm.Groups[3].Value));
				}
				foreach (Match mm in Regex.Matches(text, @"^\s*<a .*?>(.*?)</a>\s*[\d]{2,3}\s*<[^>]*>\s*(\d+)\s*</a>\s*\((.*?)\)", options))
				{
					names.Add(fix(mm.Groups[1].Value));
					opt.Add(fix(mm.Groups[2].Value));
					algo.Add(fix(mm.Groups[3].Value));
				}
				//stuff.AddRange(XElement.Parse(m.Value.ToLower()).Elements("a").Select(e=>e.Value).Where(e => e!= "*").ToList());
				//string[] values = element.Descendants("Pre").SelectMany(x => x.Elements("a").Select(y=>y.Value)).Where(x=>x != "*").ToArray();
			}

			StringBuilder code = new StringBuilder();
			code.AppendLine(GenerateCode(names, opt));
			code.AppendLine(GenerateCode(names, algo, "\""));
			code.AppendLine(GenerateCode(names, Types(names)));
			tbResult.Text = code.ToString();
			btnScrape.ToolTip = new ToolTip() { Content = "Copied to clipboard!" };
			tbResult.ToolTip = new ToolTip() { Content = "Copied to clipboard!" };
			Status.Items.Clear();
			Status.Items.Add(names.Count + " instances scraped.");
			return code.ToString();
		}

		List<string> Types(List<string> names)
		{
			string[] type1 = {"tai.a"};
			string[] type2 = {"nug","sko","wil","tho"};
			string[] type3 = {"ste","els","bur", "kra", "tai.c", "esc"};
			string[] type4 = {"tai.b"};
			Func<string,string[],bool> Find = (s,patterns) => patterns.Any(p=>Regex.IsMatch(s,p.Replace(".",@"\d+"),RegexOptions.IgnoreCase));
			return names.Select(s=> Find(s,type1) ? "\"[I]\"" : Find(s,type2) ? "\"[II]\"" : Find(s,type3) ? "\"[III]\"" : Find(s,type4) ? "\"[IV]\"" : "\"[?]\"").ToList(); 
		}

		string GenerateCode(List<string> first, List<string> second, string surroundResultWith = "")
		{
			StringBuilder code = new StringBuilder();
			if (first.Count != second.Count) throw new ArgumentException();
			for (int i = 0; i < first.Count; ++i)
			{
				code.Append("o == \"" + first[i] + "\" ? ");
				code.Append(surroundResultWith + second[i] + surroundResultWith + " : ");
			}
			code.Append("throw \"Instance not found!\";");
			return code.ToString();
		}

		private void tbResult_TextChanged(object sender, TextChangedEventArgs e)
		{
			Clipboard.Clear();
			Clipboard.SetText(tbResult.Text);
		}
	}
}
