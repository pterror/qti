import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qti.Stdlib
import "./Functions.mjs" as Functions
import "./Fetch.mjs" as Fetch

ColumnLayout {
	id: browseTab
	required property var database
	property string type: "games"
	property string price: ""
	property string category: ""
	property list<string> tags: []
	property string domain: "https://itch.io/"
	property string url: domain + [type, price, category, ...tags].filter(it => it).join("/")
	property int totalCount: 1000000
	property int lastFetchedPage: 0
	property real fetchId: 0
	property list<var> items: []
	property bool initialized: false
	Component.onCompleted: initialized = true
	onVisibleChanged: {
		if (lastFetchedPage !== 0 || !visible || !initialized) return
		fetchNextPage()
	}
	onUrlChanged: {
		if (!visible || !initialized) return
		lastFetchedPage = 0; items = []; fetchNextPage()
	}

	function unescapeHtml(html) {
		return html.replace(/&(?:amp|lt|gt|quot|#039);/g, m => ({
			"&amp;": "&", "&lt;": "<", "&gt;": ">", "&quot": "\"", "&#039;": "'",
		})[m])
	}

	function fetchNextPage() {
		const page = lastFetchedPage
		lastFetchedPage += 1
		const currentFetchId = Number(new Date())
		fetchId = currentFetchId
		Fetch.fetch(url + "?page=" + lastFetchedPage + "&format=json")
			.then(response => response.json())
			.then(({ num_items, content, page }) => {
				if (fetchId !== currentFetchId) return
				const gameHtmls = content.match(/<div[^>]+data-game_id[\s\S]+?(?=$|<div[^>]+data-game_id)/g) ?? []
				for (const gameHtml of gameHtmls) {
					const [, game_id] = gameHtml.match(/data-game_id="(\d+)"/)?.map(Number) ?? []
					const [, author_id] = gameHtml.match(/data-label="user:(\d+)"/)?.map(Number) ?? []
					const [, cover_url] = gameHtml.match(/data-lazy_src="([^\"]+)"/)
					const [, url, author_slug, game_slug] = gameHtml.match(/href="(https:[/][/]([^\"]+).itch.io[/]([^\"]+))"/) ?? []
					const [, title] = gameHtml.match(/class="title.*?">([^<]+)/)?.map(unescapeHtml) ?? []
					const [, short_text] = gameHtml.match(/class="game_text">([^<]+)/)?.map(unescapeHtml) ?? []
					const [, genre] = gameHtml.match(/class="game_genre">([^<]+)/)?.map(unescapeHtml) ?? []
					const [, author] = gameHtml.match(/class="game_author"><[^>]+>([^<]+)/)?.map(unescapeHtml) ?? []
					const windows = /title="Download for Windows"/.test(gameHtml) ? "all" : null
					const linux = /title="Download for Linux"/.test(gameHtml) ? "all" : null
					const osx = /title="Download for macOS"/.test(gameHtml) ? "all" : null
					const android = /title="Download for Android"/.test(gameHtml) ? "all" : null
					const gameInfo = {
						id: game_id, game_id, author_id,
						url, cover_url, author_slug, game_slug,
						author, title, short_text, genre,
						windows, linux, osx, android,
					}
					// TODO: query db for matching caves
					items.push(gameInfo)
				}
			})
	}

	function addToCollection(authorSlug, gameSlug) {
		const url = domain + "g/" + authorSlug + "/" + gameSlug + "/add-to-collection?source=browse"
	}

	RowLayout {
		// TODO: get result count from HTML
		spacing: 16

		Text { text: "Type" }

		ComboBox {
			model: [
				"Games", "Tools", "Game assets", "Comics", "Books", "Physical games",
				"Albums & soundtracks", "Game mods", "Everything else",
			]

			onCurrentTextChanged: {
				switch (currentText) {
					case "Games": { browseTab.type = "games"; break }
					case "Tools": { browseTab.type = "tools"; break }
					case "Game assets": { browseTab.type = "game-assets"; break }
					case "Comics": { browseTab.type = "comics"; break }
					case "Books": { browseTab.type = "books"; break }
					case "Physical games": { browseTab.type = "physical-games"; break }
					case "Albums & soundtracks": { browseTab.type = "soundtracks"; break }
					case "Game mods": { browseTab.type = "game-mods"; break }
					case "Everything else": { browseTab.type = "misc"; break }
				}
			}
		}

		Text { text: "Tags" }

		TextInput {
			Layout.fillWidth: true
			Layout.horizontalStretchFactor: 1
			onAccepted: browseTab.tags = text.split(/,\s*/).map(tag => tag.replace(/\s+/, "-"))
		}
	}

	GridView {
		id: browseView
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.fillHeight: true
		cellWidth: imageWidth + 20
		cellHeight: imageHeight + 90
		model: browseTab.items
		delegate: GameDelegate { width: browseView.cellWidth; height: browseView.cellHeight }
	}
}
